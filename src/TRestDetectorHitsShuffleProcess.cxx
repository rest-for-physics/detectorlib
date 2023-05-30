//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsShuffleProcess.cxx
///
///             Jan 2016:   First concept (Javier Galan)
///
//////////////////////////////////////////////////////////////////////////

#include "TRestDetectorHitsShuffleProcess.h"

using namespace std;

ClassImp(TRestDetectorHitsShuffleProcess);

TRestDetectorHitsShuffleProcess::TRestDetectorHitsShuffleProcess() { Initialize(); }

TRestDetectorHitsShuffleProcess::TRestDetectorHitsShuffleProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename) == -1) {
        LoadDefaultConfig();
    }
}

TRestDetectorHitsShuffleProcess::~TRestDetectorHitsShuffleProcess() {
    // delete fHitsEvent;
}

void TRestDetectorHitsShuffleProcess::LoadDefaultConfig() {
    SetName("hitsShuffleProcess");
    SetTitle("Default config");

    fIterations = 100;
}

void TRestDetectorHitsShuffleProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fHitsEvent = nullptr;

    fRandom = nullptr;
}

void TRestDetectorHitsShuffleProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name) == -1) LoadDefaultConfig();
}

void TRestDetectorHitsShuffleProcess::InitProcess() {}

TRestEvent* TRestDetectorHitsShuffleProcess::ProcessEvent(TRestEvent* inputEvent) {
    fHitsEvent = (TRestDetectorHitsEvent*)inputEvent;

    TRestHits* hits = fHitsEvent->GetHits();

    Int_t nHits = hits->GetNumberOfHits();
    if (nHits >= 2) {
        for (auto n = 0; n < fIterations; n++) {
            Int_t hit1 = (Int_t)(nHits * fRandom->Uniform(0, 1));
            Int_t hit2 = (Int_t)(nHits * fRandom->Uniform(0, 1));

            hits->SwapHits(hit1, hit2);
        }
    }
    return fHitsEvent;
}

void TRestDetectorHitsShuffleProcess::EndProcess() {}

void TRestDetectorHitsShuffleProcess::InitFromConfigFile() {
    fIterations = StringToInteger(GetParameter("iterations"));
    fRandom = new TRandom3(StringToDouble(GetParameter("seed", "0")));
}
