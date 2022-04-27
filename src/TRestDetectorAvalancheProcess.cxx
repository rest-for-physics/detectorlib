///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorAvalancheProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestDetectorAvalancheProcess
///             How to use: replace TRestDetectorAvalancheProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#include "TRestDetectorAvalancheProcess.h"

using namespace std;

#include <TRandom3.h>

ClassImp(TRestDetectorAvalancheProcess);

TRestDetectorAvalancheProcess::TRestDetectorAvalancheProcess() { Initialize(); }

TRestDetectorAvalancheProcess::TRestDetectorAvalancheProcess(char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }

    PrintMetadata();
    fGas = new TRestDetectorGas(configFilename);
}

TRestDetectorAvalancheProcess::~TRestDetectorAvalancheProcess() {
    delete fGas;
    delete fHitsOutputEvent;
}

void TRestDetectorAvalancheProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fEnergyRef = 5.9;
    fResolutionAtEref = 15.0;
    fDetectorGain = 8000.0;
}

void TRestDetectorAvalancheProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fGas = nullptr;

    fEnergyRef = 5.9;
    fResolutionAtEref = 15.0;
    fDetectorGain = 8000.0;

    fHitsInputEvent = nullptr;
    fHitsOutputEvent = new TRestDetectorHitsEvent();
}

void TRestDetectorAvalancheProcess::LoadConfig(string configFilename) {
    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();

    PrintMetadata();
    fGas = new TRestDetectorGas(configFilename.c_str());
    fGas->PrintMetadata();
}

void TRestDetectorAvalancheProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();

    if (fGas == nullptr) cout << "REST ERRORRRR : Gas has not been initialized" << endl;
}

TRestEvent* TRestDetectorAvalancheProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsInputEvent = (TRestDetectorHitsEvent*)evInput;

    Double_t fW = fGas->GetWvalue();
    Double_t gain, totelectrons = 0;

    Double_t eDep = fHitsInputEvent->GetTotalEnergy() * fW / 1000.0;
    Double_t eRes = fResolutionAtEref * TMath::Sqrt(fEnergyRef / eDep) / 2.35 / 100.0;

    cout << "Initial electrons " << fHitsInputEvent->GetTotalEnergy() << " ; eDep " << eDep
         << " keV; resolution " << eRes * 2.35 * 100 << " fwhm" << endl;

    TRandom3* rnd = new TRandom3(0);

    for (int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++) {
        gain = fDetectorGain * rnd->Gaus(1.0, eRes);

        // The electronics gain is applied.
        // gain = gain * 4096.0 / fElectronicsGain;

        totelectrons += gain;

        fHitsOutputEvent->AddHit(fHitsInputEvent->GetX(hit), fHitsInputEvent->GetY(hit),
                                 fHitsInputEvent->GetZ(hit), fHitsInputEvent->GetEnergy(hit) * gain);
    }

    delete rnd;

    if (fHitsOutputEvent->GetNumberOfHits() == 0) return nullptr;

    cout << "Initial: " << fHitsInputEvent->GetNumberOfHits() << "e-s, and amplified: " << totelectrons
         << " e-s : " << endl;
    return fHitsOutputEvent;
}

void TRestDetectorAvalancheProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

void TRestDetectorAvalancheProcess::InitFromConfigFile() {
    fEnergyRef = GetDblParameterWithUnits("energyReference");
    fResolutionAtEref = StringToDouble(GetParameter("resolutionReference"));
    fDetectorGain = StringToDouble(GetParameter("detectorGain"));
}
