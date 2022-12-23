///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsNormalizationProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestDetectorHitsNormalizationProcess
///             How to use: replace TRestDetectorHitsNormalizationProcess by your name,
///             fill the required functions following instructions and add all
///             needed additional members and funcionality
///
///             sep 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestDetectorHitsNormalizationProcess.h"

using namespace std;

ClassImp(TRestDetectorHitsNormalizationProcess);

TRestDetectorHitsNormalizationProcess::TRestDetectorHitsNormalizationProcess() { Initialize(); }

TRestDetectorHitsNormalizationProcess::TRestDetectorHitsNormalizationProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }

    PrintMetadata();
}

TRestDetectorHitsNormalizationProcess::~TRestDetectorHitsNormalizationProcess() {
    delete fHitsOutputEvent;
    // TRestDetectorHitsNormalizationProcess destructor
}

void TRestDetectorHitsNormalizationProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fFactor = 5.9;
}

void TRestDetectorHitsNormalizationProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fFactor = 1.;

    fHitsInputEvent = nullptr;
    fHitsOutputEvent = new TRestDetectorHitsEvent();
}

void TRestDetectorHitsNormalizationProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();

    PrintMetadata();
}

void TRestDetectorHitsNormalizationProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

TRestEvent* TRestDetectorHitsNormalizationProcess::ProcessEvent(TRestEvent* inputEvent) {
    fHitsInputEvent = (TRestDetectorHitsEvent*)inputEvent;
    fHitsOutputEvent->SetEventInfo(fHitsInputEvent);

    for (unsigned int hit = 0; hit < fHitsInputEvent->GetNumberOfHits(); hit++)
        fHitsOutputEvent->AddHit(fHitsInputEvent->GetX(hit), fHitsInputEvent->GetY(hit),
                                 fHitsInputEvent->GetZ(hit), fHitsInputEvent->GetEnergy(hit) * fFactor,
                                 fHitsInputEvent->GetTime(hit), fHitsInputEvent->GetType(hit));

    if (this->GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
        cout << "TRestDetectorHitsNormalizationProcess. Hits added : " << fHitsOutputEvent->GetNumberOfHits()
             << endl;
        cout << "TRestDetectorHitsNormalizationProcess. Hits total energy : " << fHitsOutputEvent->GetEnergy()
             << endl;
    }

    return fHitsOutputEvent;
}

void TRestDetectorHitsNormalizationProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

void TRestDetectorHitsNormalizationProcess::InitFromConfigFile() {
    fFactor = StringToDouble(GetParameter("normFactor", "1"));
}
