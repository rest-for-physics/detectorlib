///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorFiducializationProcess.cxx
///
///
///             First implementation of electron diffusion process into REST_v2
///             Date : Mar/2017
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestDetectorFiducializationProcess.h"

using namespace std;

ClassImp(TRestDetectorFiducializationProcess);

TRestDetectorFiducializationProcess::TRestDetectorFiducializationProcess() { Initialize(); }

TRestDetectorFiducializationProcess::TRestDetectorFiducializationProcess(const char* configFilename) {
    Initialize();
    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }
}

TRestDetectorFiducializationProcess::~TRestDetectorFiducializationProcess() { delete fOutputHitsEvent; }

void TRestDetectorFiducializationProcess::LoadDefaultConfig() { SetTitle("Default config"); }

void TRestDetectorFiducializationProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fOutputHitsEvent = new TRestDetectorHitsEvent();
    fInputHitsEvent = nullptr;

    fReadout = nullptr;
}

void TRestDetectorFiducializationProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

void TRestDetectorFiducializationProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();
    if (fReadout == nullptr) {
        cout << "REST ERROR: Readout has not been initialized" << endl;
        exit(-1);
    }
}

TRestEvent* TRestDetectorFiducializationProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)inputEvent;

    Int_t nHits = fInputHitsEvent->GetNumberOfHits();
    if (nHits <= 0) return nullptr;

    TRestHits* hits = fInputHitsEvent->GetHits();
    for (auto n = 0; n < nHits; n++) {
        Double_t eDep = hits->GetEnergy(n);

        const Double_t x = hits->GetX(n);
        const Double_t y = hits->GetY(n);
        const Double_t z = hits->GetZ(n);

        const Double_t t = hits->GetTime(n);
        auto type = hits->GetType(n);

        for (auto p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
            TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlane(p);

            if (plane->GetModuleIDFromPosition(TVector3(x, y, z)) >= 0)
                fOutputHitsEvent->AddHit(x, y, z, eDep, t, type);
        }
    }

    if (fOutputHitsEvent->GetNumberOfHits() == 0) return nullptr;

    if (this->GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
        cout << "TRestDetectorFiducializationProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits()
             << endl;
        cout << "TRestDetectorFiducializationProcess. Hits total energy : " << fOutputHitsEvent->GetEnergy()
             << endl;
    }

    return fOutputHitsEvent;
}

void TRestDetectorFiducializationProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

void TRestDetectorFiducializationProcess::InitFromConfigFile() {}
