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

#ifndef RestCore_TRestDetectorHitsVolumeGainProcess
#define RestCore_TRestDetectorHitsVolumeGainProcess

#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TGeoNode.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestEventProcess.h>

#include <map>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/// A process to scale the hits energy by a gain factor depending on the geometry volume
class TRestDetectorHitsVolumeGainProcess : public TRestEventProcess {
   private:
    /// A pointer to the process input event
    TRestDetectorHitsEvent* fInputEvent = nullptr;  //!

    /// A pointer to the process output event
    TRestDetectorHitsEvent* fOutputEvent = nullptr;  //!

    /// The geometry used to locate the hits. It is never owned by this process
    TGeoManager* fGeoManager = nullptr;  //!

    /// A navigator private to the thread calling ProcessEvent. It is owned by fGeoManager
    TGeoNavigator* fNavigator = nullptr;  //!

    /// The thread fNavigator belongs to. The navigator is rebuilt whenever it changes
    std::thread::id fNavigatorThreadId;  //!

    /// A description of where fGeoManager was taken from, just for reporting
    std::string fGeometrySource = "";  //!

    /// Lazily filled volume path to gain cache. It avoids repeating the name matching on
    /// each hit. The path is the key because it is the only thing identifying a volume:
    /// when a volume is placed several times ROOT reuses one node object for all the copies
    std::map<std::string, Double_t> fPathGainCache;  //!

    void InitFromConfigFile() override;
    void Initialize() override;

    Double_t GetGain(Double_t x, Double_t y, Double_t z);
    Double_t MatchVolume(const std::string& path, const std::string& logicalName) const;
    std::vector<std::string> GetPathVolumes() const;
    std::set<std::string> GetAllVolumePaths() const;
    void LoadGeometry();
    void ValidateVolumeExpressions();
    void UpdateNavigator();

   protected:
    /// The (volume name expression, gain) pairs given by the user, in RML declaration order
    std::vector<std::pair<std::string, Double_t>> fVolumeGains;

    /// Gain given to hits inside a volume matching no expression, and to hits outside the geometry
    Double_t fDefaultGain = 1.0;  //<

    /// If true, hits matching no volume expression are removed instead of scaled by fDefaultGain
    Bool_t fRemoveUnmatchedHits = false;  //<

    /// An optional GDML file. When given it takes priority over the input file geometry
    std::string fGdmlFilename = "";  //<

    /// The string joining the volume names of a path, as TRestGeant4Metadata defines it
    std::string fVolPathSeparator = "_";  //<

   public:
    RESTValue GetInputEvent() const override { return fInputEvent; }
    RESTValue GetOutputEvent() const override { return fOutputEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    const char* GetProcessName() const override { return "hitsVolumeGain"; }

    inline Double_t GetDefaultGain() const { return fDefaultGain; }
    inline Bool_t GetRemoveUnmatchedHits() const { return fRemoveUnmatchedHits; }
    inline std::string GetGdmlFilename() const { return fGdmlFilename; }
    inline std::string GetVolPathSeparator() const { return fVolPathSeparator; }
    inline std::vector<std::pair<std::string, Double_t>> GetVolumeGains() const { return fVolumeGains; }

    TRestDetectorHitsVolumeGainProcess();
    TRestDetectorHitsVolumeGainProcess(const char* configFilename);
    ~TRestDetectorHitsVolumeGainProcess();

    ClassDefOverride(TRestDetectorHitsVolumeGainProcess, 1);
};
#endif
