///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsSmearingProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestDetectorHitsSmearingProcess
///             How to use: replace TRestDetectorHitsSmearingProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#include "TRestDetectorHitsSmearingProcess.h"

using namespace std;

ClassImp(TRestDetectorHitsSmearingProcess);

TRestDetectorHitsSmearingProcess::TRestDetectorHitsSmearingProcess() { Initialize(); }

TRestDetectorHitsSmearingProcess::TRestDetectorHitsSmearingProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName)) {
        LoadDefaultConfig();
    }

    PrintMetadata();

    // TRestDetectorHitsSmearingProcess default constructor
}

TRestDetectorHitsSmearingProcess::~TRestDetectorHitsSmearingProcess() {
    delete fHitsOutputEvent;
    // TRestDetectorHitsSmearingProcess destructor
}

void TRestDetectorHitsSmearingProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fEnergyRef = 5.9;
    fResolutionAtERef = 15.0;
}

void TRestDetectorHitsSmearingProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fEnergyRef = 5.9;
    fResolutionAtERef = 15.0;

    fHitsInputEvent = nullptr;
    fHitsOutputEvent = new TRestDetectorHitsEvent();

    fRandom = nullptr;
}

void TRestDetectorHitsSmearingProcess::LoadConfig(const string& cfgFilename, const string& name) {
    if (LoadConfigFromFile(cfgFilename, name)) {
        LoadDefaultConfig();
    }

    PrintMetadata();

    fGas = GetMetadata<TRestDetectorGas>();
}

void TRestDetectorHitsSmearingProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

TRestEvent* TRestDetectorHitsSmearingProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsInputEvent = (TRestDetectorHitsEvent*)evInput;
    fHitsOutputEvent->SetEventInfo(fHitsInputEvent);

    Double_t eDep = fHitsInputEvent->GetTotalEnergy();
    Double_t eRes = fResolutionAtERef * TMath::Sqrt(fEnergyRef / eDep) / 2.35 / 100.0;

    Double_t gain = fRandom->Gaus(1.0, eRes);
    for (int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++)
        fHitsOutputEvent->AddHit(fHitsInputEvent->GetX(hit), fHitsInputEvent->GetY(hit),
                                 fHitsInputEvent->GetZ(hit), fHitsInputEvent->GetEnergy(hit) * gain,
                                 fHitsInputEvent->GetTime(hit), fHitsInputEvent->GetType(hit));

    return fHitsOutputEvent;
}

void TRestDetectorHitsSmearingProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

void TRestDetectorHitsSmearingProcess::InitFromConfigFile() {
    fEnergyRef = GetDblParameterWithUnits("energyReference");
    fResolutionAtERef = StringToDouble(GetParameter("resolutionReference"));
    fRandom = new TRandom3(StringToDouble(GetParameter("seed", "0")));
}
