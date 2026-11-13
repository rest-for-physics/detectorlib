/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// TRestDetectorHitsVolumeGainProcess multiplies the energy of each hit
/// inside a TRestDetectorHitsEvent by a gain factor that depends on the
/// geometry volume containing that hit.
///
/// The geometry is the one used by the restG4 simulation. restG4 writes it
/// to its output file as a TGeoManager stored under the key `Geometry`, and
/// this process will recover it from the input file without any additional
/// configuration. When the input file contains no TGeoManager, or when a
/// different geometry is wanted, the user may give the path to a GDML file
/// through the `gdmlFilename` parameter. **If `gdmlFilename` is given it
/// always takes priority over the geometry stored in the input file.**
///
/// The gain assigned to each volume is given with one `volume` element per
/// volume. The `name` field accepts wildcards (`*` and `?`), and the first
/// `volume` element matching wins, so more specific expressions should be
/// placed first.
///
/// Volumes are named by their **path**: the names of the physical volumes
/// containing the volume, joined by `volPathSeparator` (`_` by default), with
/// the world left out. For instance `vessel_gas`, or `moduleLeft_plate` for a
/// plate placed inside an assembly placed as `moduleLeft`.
///
/// This is exactly the naming TRestGeant4GeometryInfo builds, so the volume
/// names used anywhere else in a Geant4 analysis can be pasted here unchanged.
/// It is also the only naming that tells apart the several placements of one
/// volume, since `moduleLeft_plate` and `moduleRight_plate` are different
/// paths for what ROOT stores as a single node.
///
/// The logical volume name, the `name` of the GDML `volume` or `assembly`
/// element such as `gasVolume`, is accepted as well. It matches every
/// placement of that volume at once, which is handy when the gain does not
/// depend on where the volume sits. TRestGeant4GeometryInfo keeps the same
/// distinction between physical and logical names.
///
/// \note The whole name must be matched, not a part of it: `gas` will not
/// match a volume named `gasVolume`, `gas*` will. This differs from the
/// regular expressions used by TRestGeant4QuenchingProcess.
///
/// \code
///    <addProcess type="TRestDetectorHitsVolumeGainProcess" name="volGain"
///                defaultGain="1" removeUnmatchedHits="false">
///        <volume name="vessel_gas" gain="1.0"/>
///        <volume name="moduleLeft_plate" gain="0.35"/>
///        <volume name="moduleRight*" gain="0.80"/>
///    </addProcess>
/// \endcode
///
/// Hits inside a volume that matches no expression, hits falling outside the
/// geometry, and hits with a non-XYZ type (whose position is not fully
/// determined) are all scaled by `defaultGain`, which is 1 by default and
/// therefore leaves them untouched. Setting `removeUnmatchedHits` to true
/// removes those hits from the output event instead.
///
/// \note REST hit coordinates are given in mm while the ROOT geometry uses
/// cm. The conversion is done internally by this process.
///
/// \warning ROOT keeps a single geometry at a time: building a TGeoManager
/// destroys the previous one. This process therefore loads the geometry only
/// once per job and shares it between the parallel instances of the process,
/// and a `gdmlFilename` given here will replace any geometry already loaded.
///
/// \warning ROOT geometry navigation is not thread safe out of the box.
/// This process enables the multithreading support of the TGeoManager and
/// gives one TGeoNavigator to each processing thread. The geometry itself
/// is loaded only once and shared by every thread.
///
/// Volumes are looked up from the inside out: the volume the hit falls in is
/// tried first, and if it defines no gain the volumes containing it are tried,
/// up to the world. So a gain given to a mother volume applies to all of its
/// daughters unless a daughter defines its own, and a gain given to an
/// assembly applies to everything placed inside it. This is the only way to
/// address an assembly, since assemblies have no shape of their own and the
/// navigator never stops at one.
///
/// \note The gain of each volume is resolved once per path and cached, so the
/// name matching does not run again for every hit.
///
/// \note The ROOT GDML importer usually appends the copy number to the
/// physical volume names, so a volume declared as `gasVolume` in the GDML
/// file may be named `gasVolume_0` in the TGeoManager. Either use the
/// logical volume name or a wildcard expression such as `gasVolume*`. Run
/// the process in debug mode to print the names actually found.
///
/// The process adds the observables `energyBefore` and `energyAfter`, the
/// total hits energy before and after applying the gains, and `removedHits`,
/// the number of hits removed when `removeUnmatchedHits` is enabled.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2026-September: First implementation of TRestDetectorHitsVolumeGainProcess
/// \author Alvaro Ezquerro
///
/// \class TRestDetectorHitsVolumeGainProcess
///
/// <hr>
///
#include "TRestDetectorHitsVolumeGainProcess.h"

#include <TDirectory.h>
#include <TFile.h>
#include <TGeoVolume.h>
#include <TKey.h>
#include <TRestGDMLParser.h>
#include <TRestManager.h>
#include <TRestProcessRunner.h>
#include <TRestTools.h>

#include <limits>
#include <set>
#include <stdexcept>
#include <thread>

using namespace std;

ClassImp(TRestDetectorHitsVolumeGainProcess);

/// Returned by GetGain when the hit could not be assigned to any user volume
const Double_t kUnmatchedGain = numeric_limits<Double_t>::quiet_NaN();

/// Geometries already loaded by this process, indexed by the source they came from.
///
/// Constructing a TGeoManager **deletes the previous gGeoManager** (see TGeoManager::Init,
/// which prints "Deleting previous geometry"). That makes reloading a geometry actively
/// dangerous here: TRestProcessRunner creates one process instance per thread
/// (TRestProcessRunner.cxx:232-241) and TRestThread calls InitProcess() twice on each of
/// them (TRestThread.cxx:285 and :389, with a test run in between), so a naive "load it in
/// InitProcess()" would leave every earlier instance holding a dangling pointer.
///
/// The geometry is therefore loaded at most once and shared by every instance. InitProcess()
/// always runs on the main thread (TRestThread::PrepareToProcess, called from
/// TRestProcessRunner.cxx:359-361 before any thread is started), so no locking is needed.
/// The geometries are never deleted, as ROOT does with gGeoManager.
namespace {
map<string, TGeoManager*> gLoadedGeometries;
}

TRestDetectorHitsVolumeGainProcess::TRestDetectorHitsVolumeGainProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param configFilename A const char* giving the path to an RML file.
///
TRestDetectorHitsVolumeGainProcess::TRestDetectorHitsVolumeGainProcess(const char* configFilename) {
    Initialize();
    LoadConfigFromFile(configFilename);
}

TRestDetectorHitsVolumeGainProcess::~TRestDetectorHitsVolumeGainProcess() {
    // fNavigator is owned by fGeoManager, and fGeoManager is shared, so neither is deleted here
    delete fOutputEvent;
}

void TRestDetectorHitsVolumeGainProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputEvent = nullptr;
    fOutputEvent = new TRestDetectorHitsEvent();
}

void TRestDetectorHitsVolumeGainProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();

    fVolumeGains.clear();

    TiXmlElement* volumeElement = GetElement("volume");
    while (volumeElement != nullptr) {
        const string volumeName = GetParameter("name", volumeElement, "");
        if (volumeName.empty()) {
            RESTError << "TRestDetectorHitsVolumeGainProcess: found a <volume> element with no `name` "
                      << "field defined" << RESTendl;
            throw runtime_error("TRestDetectorHitsVolumeGainProcess: <volume> element with no name");
        }

        fVolumeGains.emplace_back(volumeName, StringToDouble(GetParameter("gain", volumeElement, "1")));

        volumeElement = GetNextElement(volumeElement);
    }
}

///////////////////////////////////////////////
/// \brief Recovers the geometry into fGeoManager.
///
/// A GDML file given by the user takes priority over the geometry stored inside the
/// input file. When no GDML file is given the geometry already in memory is preferred,
/// and only if there is none the keys of the input file are searched.
///
/// The geometry is loaded at most once per job and shared by every parallel instance of
/// this process. See the note on gLoadedGeometries above: building a second TGeoManager
/// would destroy the first one.
///
void TRestDetectorHitsVolumeGainProcess::LoadGeometry() {
    // TFile::Get and TGeoManager::Import change gDirectory, and the caller may have left
    // the output file as the current directory (TRestThread.cxx:387)
    TDirectory::TContext directoryContext;

    const string cacheKey = fGdmlFilename.empty() ? "<input file>" : "gdml:" + fGdmlFilename;

    const auto cached = gLoadedGeometries.find(cacheKey);
    if (cached != gLoadedGeometries.end()) {
        fGeoManager = cached->second;
        return;
    }

    if (!fGdmlFilename.empty()) {
        string filename = fGdmlFilename;
        if (!TRestTools::fileExists(filename)) filename = SearchFile(fGdmlFilename);
        if (!TRestTools::fileExists(filename)) {
            RESTError << "TRestDetectorHitsVolumeGainProcess: the GDML file given at `gdmlFilename` "
                      << "does not exist : " << fGdmlFilename << RESTendl;
            throw runtime_error("TRestDetectorHitsVolumeGainProcess: GDML file not found : " + fGdmlFilename);
        }

        if (gGeoManager != nullptr) {
            RESTInfo << "TRestDetectorHitsVolumeGainProcess: a geometry was already loaded, but it "
                     << "will be replaced by the GDML file given at `gdmlFilename`" << RESTendl;
        }

        // TRestGDMLParser::CreateGeoManager() moves into the preprocessed GDML directory
        // before importing, which is needed for GDML files using relative references.
        // The parser is intentionally not deleted: its destructor frees a TiXmlElement it
        // does not own and crashes. This is what restG4 does too (Application.cxx:335-338)
        auto parser = new TRestGDMLParser();
        parser->Load(filename);
        fGeoManager = parser->CreateGeoManager();

        if (fGeoManager == nullptr) {
            RESTError << "TRestDetectorHitsVolumeGainProcess: unable to build a geometry out of the "
                      << "GDML file : " << filename << RESTendl;
            throw runtime_error("TRestDetectorHitsVolumeGainProcess: could not load GDML file " + filename);
        }

        fGeometrySource = "GDML file " + filename;
        gLoadedGeometries[cacheKey] = fGeoManager;
        return;
    }

    // TRestRun::ReadInputFileMetadata() already reads every key of the input file, so when the
    // file holds a geometry it is in memory by now and gGeoManager points at it. Reading the key
    // again would build a second TGeoManager and delete this one, so reuse it instead.
    if (gGeoManager != nullptr) {
        fGeoManager = gGeoManager;
        fGeometrySource = (string) "the geometry already in memory (`" + fGeoManager->GetName() + "`)";
        gLoadedGeometries[cacheKey] = fGeoManager;
        return;
    }

    TFile* inputFile = fRunInfo != nullptr ? fRunInfo->GetInputFile() : nullptr;
    if (inputFile != nullptr) {
        // Prefer the `Geometry` key written by restG4, but accept any TGeoManager key
        string geometryKey;
        TIter next(inputFile->GetListOfKeys());
        while (TKey* key = (TKey*)next()) {
            if ((TString)key->GetClassName() != (TString) "TGeoManager") continue;
            if (geometryKey.empty() || (TString)key->GetName() == (TString) "Geometry")
                geometryKey = key->GetName();
        }

        if (!geometryKey.empty()) {
            fGeoManager = inputFile->Get<TGeoManager>(geometryKey.c_str());
            if (fGeoManager != nullptr) {
                fGeometrySource = "the `" + geometryKey + "` key of " + inputFile->GetName();
                gLoadedGeometries[cacheKey] = fGeoManager;
                return;
            }
        }
    }

    RESTError << "TRestDetectorHitsVolumeGainProcess: no geometry is available. The input file "
              << "contains no object of class TGeoManager (restG4 writes it under the key "
              << "`Geometry`) and no `gdmlFilename` parameter was given" << RESTendl;
    throw runtime_error("TRestDetectorHitsVolumeGainProcess: no geometry available");
}

///////////////////////////////////////////////
/// \brief Warns about user volume expressions matching no volume of the geometry.
///
void TRestDetectorHitsVolumeGainProcess::ValidateVolumeExpressions() {
    set<string> names;

    if (fGeoManager->GetTopNode() != nullptr) {
        names.insert(fGeoManager->GetTopNode()->GetName());
    }

    TIter nextVolume(fGeoManager->GetListOfVolumes());
    while (TGeoVolume* volume = (TGeoVolume*)nextVolume()) {
        names.insert(volume->GetName());

        TIter nextNode(volume->GetNodes());
        while (TGeoNode* node = (TGeoNode*)nextNode()) {
            names.insert(node->GetName());
        }
    }

    RESTDebug << "TRestDetectorHitsVolumeGainProcess: volume names found at the geometry" << RESTendl;
    for (const auto& name : names) {
        RESTDebug << " - " << name << RESTendl;
    }

    // Only expanded when some expression did not match a plain name, since the expanded tree
    // can be much larger than the list of volumes
    std::set<string> paths;
    bool pathsBuilt = false;

    for (const auto& [expression, gain] : fVolumeGains) {
        bool found = false;
        for (const auto& name : names) {
            if (MatchString(name, expression)) {
                found = true;
                break;
            }
        }

        if (!found) {
            if (!pathsBuilt) {
                paths = GetAllVolumePaths();
                pathsBuilt = true;
            }
            for (const auto& path : paths) {
                if (MatchString(path, expression)) {
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            RESTWarning << "TRestDetectorHitsVolumeGainProcess: no volume of the geometry matches the "
                        << "expression : " << expression << RESTendl;
        }
    }
}

///////////////////////////////////////////////
/// \brief Returns every volume path of the geometry, using the TRestGeant4GeometryInfo naming.
///
/// This expands the whole node tree, so it is only used to tell the user which names are
/// available when one of the expressions did not match anything.
///
std::set<string> TRestDetectorHitsVolumeGainProcess::GetAllVolumePaths() const {
    std::set<string> paths;

    TGeoIterator iterator(fGeoManager->GetTopVolume());
    TGeoNode* node;
    std::vector<string> branch;
    while ((node = (TGeoNode*)iterator.Next()) != nullptr) {
        branch.resize(iterator.GetLevel());
        branch[iterator.GetLevel() - 1] = node->GetName();

        string path;
        for (const auto& volume : branch) path += (path.empty() ? "" : fVolPathSeparator) + volume;
        paths.insert(path);

        if (paths.size() > 200000) break;  // do not blow up on a huge geometry
    }

    return paths;
}

void TRestDetectorHitsVolumeGainProcess::InitProcess() {
    // TRestThread::PrepareToProcess calls InitProcess() twice on each instance
    // (TRestThread.cxx:285 and :389, with a test run in between). Reloading the geometry on
    // the second call would destroy the one the test run navigated, so make this idempotent.
    if (fGeoManager != nullptr) return;

    LoadGeometry();

    RESTInfo << "TRestDetectorHitsVolumeGainProcess: geometry taken from " << fGeometrySource << RESTendl;

    if (!fGeoManager->IsClosed()) fGeoManager->CloseGeometry();

    if (fGeoManager->GetTopNode() == nullptr) {
        RESTError << "TRestDetectorHitsVolumeGainProcess: the geometry has no top node" << RESTendl;
        throw runtime_error("TRestDetectorHitsVolumeGainProcess: the geometry has no top node");
    }

    if (fVolumeGains.empty()) {
        RESTWarning << "TRestDetectorHitsVolumeGainProcess: no <volume> element was defined, every hit "
                    << "will get the default gain (" << fDefaultGain << ")" << RESTendl;
    }

    ValidateVolumeExpressions();

    // ROOT geometry navigation is only thread safe when the geometry knows how many threads
    // will navigate it: TGeoVolume keeps per-thread scratch data indexed by TGeoManager::ThreadId(),
    // which always returns 0 unless multithreading has been enabled. This has to happen on the main
    // thread, before TRestThread::StartThread() launches the workers, and only once, because
    // SetMaxThreads() clears the thread data of the navigators created so far.
    if (!fGeoManager->IsMultiThread()) {
        Int_t threads = 15;  // TRestProcessRunner.cxx:191-192 caps the thread number at 15
        if (fHostmgr != nullptr && fHostmgr->GetProcessRunner() != nullptr)
            threads = fHostmgr->GetProcessRunner()->GetNThreads();
        // +2 leaves room for the main thread, which runs the test run and standalone macros
        fGeoManager->SetMaxThreads(threads + 2);
    }

    fNavigator = nullptr;
    fPathGainCache.clear();
}

///////////////////////////////////////////////
/// \brief Makes sure fNavigator belongs to the thread calling ProcessEvent.
///
/// InitProcess() runs on the main thread while ProcessEvent() runs on a worker thread
/// (TRestThread::StartThread), and the test run calls ProcessEvent() on the main thread
/// beforehand, so the navigator cannot be created once and for all in InitProcess().
///
void TRestDetectorHitsVolumeGainProcess::UpdateNavigator() {
    const auto thisThread = this_thread::get_id();
    if (fNavigator != nullptr && fNavigatorThreadId == thisThread) return;

    // The navigator is owned by fGeoManager, which keeps one list per thread
    fNavigator = fGeoManager->AddNavigator();
    fNavigatorThreadId = thisThread;
}

///////////////////////////////////////////////
/// \brief Returns the gain defined for a volume, given its path and its logical volume
/// name, or kUnmatchedGain when no user expression matches either of them.
///
/// The user expressions are evaluated in the order they were declared, so the first one
/// matching wins. Both names are tried for each expression before moving on to the next,
/// so the declaration order decides and not whether a path or a logical name was written.
///
Double_t TRestDetectorHitsVolumeGainProcess::MatchVolume(const string& path,
                                                         const string& logicalName) const {
    for (const auto& [expression, value] : fVolumeGains) {
        if (!path.empty() && MatchString(path, expression)) return value;
        if (!logicalName.empty() && MatchString(logicalName, expression)) return value;
    }

    return kUnmatchedGain;
}

///////////////////////////////////////////////
/// \brief Returns the volume names of the path the navigator is currently at, from the
/// one just below the world down to the volume containing the point.
///
/// The world is left out, so that joining the result with fVolPathSeparator gives the
/// same name TRestGeant4GeometryInfo builds for that volume.
///
std::vector<string> TRestDetectorHitsVolumeGainProcess::GetPathVolumes() const {
    std::vector<string> volumes;

    const string path = fNavigator->GetPath();  // e.g. /world_1/vessel/gas
    size_t position = path.find('/');
    while (position != string::npos) {
        const size_t next = path.find('/', position + 1);
        const string volume =
            path.substr(position + 1, next == string::npos ? string::npos : next - position - 1);
        if (!volume.empty()) volumes.push_back(volume);
        position = next;
    }

    // The first one is the world, which TRestGeant4GeometryInfo does not include
    if (!volumes.empty()) volumes.erase(volumes.begin());

    return volumes;
}

///////////////////////////////////////////////
/// \brief Returns the gain to be applied at the given position, given in mm.
///
/// It returns kUnmatchedGain when the position is outside the geometry or when neither
/// the volume containing it nor any of the volumes containing that one define a gain.
///
Double_t TRestDetectorHitsVolumeGainProcess::GetGain(Double_t x, Double_t y, Double_t z) {
    // REST hits are given in mm while the ROOT geometry uses cm
    if (fNavigator->FindNode(x / 10., y / 10., z / 10.) == nullptr) return kUnmatchedGain;

    // The path of every volume the point is inside of, from the one just below the world
    // down to the one containing it, which is the naming TRestGeant4GeometryInfo uses
    std::vector<string> paths;
    string path;
    for (const auto& volume : GetPathVolumes()) {
        path += (path.empty() ? "" : fVolPathSeparator) + volume;
        paths.push_back(path);
    }

    const auto cached = fPathGainCache.find(path);
    if (cached != fPathGainCache.end()) return cached->second;

    Double_t gain = kUnmatchedGain;

    // The innermost volume takes precedence. When it defines no gain the volumes containing
    // it are tried, from the innermost outwards, which is just the path getting shorter.
    // This is what makes assemblies usable: they have no shape of their own, so the
    // navigator never stops at one, only at one of the volumes placed inside it
    const Int_t deepest = fNavigator->GetLevel();
    for (Int_t level = deepest; level >= 1 && TMath::IsNaN(gain); level--) {
        TGeoNode* node = fNavigator->GetMother(deepest - level);
        const string logicalName =
            node != nullptr && node->GetVolume() != nullptr ? node->GetVolume()->GetName() : "";

        gain = MatchVolume(level <= (Int_t)paths.size() ? paths[level - 1] : "", logicalName);
    }

    fPathGainCache[path] = gain;
    return gain;
}

TRestEvent* TRestDetectorHitsVolumeGainProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = (TRestDetectorHitsEvent*)inputEvent;

    // TRestEventProcess::BeginOfEventProcess already does this during a process chain, but not
    // when the process is driven by hand from a macro
    fOutputEvent->Initialize();
    fOutputEvent->SetEventInfo(fInputEvent);

    UpdateNavigator();

    Double_t energyBefore = 0;
    Double_t energyAfter = 0;
    Int_t removedHits = 0;

    for (unsigned int n = 0; n < fInputEvent->GetNumberOfHits(); n++) {
        const Double_t x = fInputEvent->GetX(n);
        const Double_t y = fInputEvent->GetY(n);
        const Double_t z = fInputEvent->GetZ(n);
        const Double_t energy = fInputEvent->GetEnergy(n);
        const auto type = fInputEvent->GetType(n);

        energyBefore += energy;

        // Only XYZ hits have a fully determined position, the others cannot be located
        Double_t gain = type == XYZ ? GetGain(x, y, z) : kUnmatchedGain;

        if (TMath::IsNaN(gain)) {
            if (fRemoveUnmatchedHits) {
                removedHits++;
                continue;
            }
            gain = fDefaultGain;
        }

        energyAfter += energy * gain;
        fOutputEvent->AddHit(x, y, z, energy * gain, fInputEvent->GetTime(n), type);
    }

    RESTDebug << "TRestDetectorHitsVolumeGainProcess: energy " << energyBefore << " keV -> " << energyAfter
              << " keV, " << removedHits << " hits removed" << RESTendl;

    SetObservableValue("energyBefore", energyBefore);
    SetObservableValue("energyAfter", energyAfter);
    SetObservableValue("removedHits", removedHits);

    return fOutputEvent;
}

void TRestDetectorHitsVolumeGainProcess::PrintMetadata() {
    BeginPrintProcess();

    if (!fGdmlFilename.empty()) RESTMetadata << " - GDML file : " << fGdmlFilename << RESTendl;
    if (!fGeometrySource.empty()) RESTMetadata << " - Geometry taken from : " << fGeometrySource << RESTendl;

    RESTMetadata << " - Default gain : " << fDefaultGain << RESTendl;
    RESTMetadata << " - Remove unmatched hits : " << (fRemoveUnmatchedHits ? "true" : "false") << RESTendl;

    RESTMetadata << " " << RESTendl;
    RESTMetadata << " Volume gains : " << RESTendl;
    if (fVolumeGains.empty()) {
        RESTMetadata << "  No volume gains defined!" << RESTendl;
    }
    for (const auto& [expression, gain] : fVolumeGains) {
        RESTMetadata << "  - " << expression << " : " << gain << RESTendl;
    }

    EndPrintProcess();
}
