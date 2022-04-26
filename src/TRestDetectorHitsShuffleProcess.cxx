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

ClassImp(TRestDetectorHitsShuffleProcess)
    //______________________________________________________________________________
    TRestDetectorHitsShuffleProcess::TRestDetectorHitsShuffleProcess() {
    Initialize();
}

TRestDetectorHitsShuffleProcess::TRestDetectorHitsShuffleProcess(char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename) == -1) LoadDefaultConfig();
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

    fHitsEvent = NULL;

    fRandom = NULL;
}

void TRestDetectorHitsShuffleProcess::LoadConfig(std::string configFilename, std::string name) {
    if (LoadConfigFromFile(configFilename, name) == -1) LoadDefaultConfig();
}

void TRestDetectorHitsShuffleProcess::InitProcess() {}

TRestEvent* TRestDetectorHitsShuffleProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsEvent = (TRestDetectorHitsEvent*)evInput;

    TRestHits* hits = fHitsEvent->GetHits();

    Int_t nHits = hits->GetNumberOfHits();
    if (nHits >= 2) {
        for (int n = 0; n < fIterations; n++) {
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
