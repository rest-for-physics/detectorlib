///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsRotateAndTranslateProcess.cxx
///
///             Template to use to design "event process" classes inherited from
///             TRestDetectorHitsRotateAndTranslateProcess
///             How to use: replace TRestDetectorHitsRotateAndTranslateProcess by your
///             name, fill the required functions following instructions and add
///             all needed additional members and funcionality
///
///             march 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#include "TRestDetectorHitsRotateAndTranslateProcess.h"

using namespace std;

#include <TRandom3.h>

ClassImp(TRestDetectorHitsRotateAndTranslateProcess);

TRestDetectorHitsRotateAndTranslateProcess::TRestDetectorHitsRotateAndTranslateProcess() { Initialize(); }

TRestDetectorHitsRotateAndTranslateProcess::TRestDetectorHitsRotateAndTranslateProcess(
    const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }

    PrintMetadata();
}

TRestDetectorHitsRotateAndTranslateProcess::~TRestDetectorHitsRotateAndTranslateProcess() {
    // TRestDetectorHitsRotateAndTranslateProcess destructor
}

void TRestDetectorHitsRotateAndTranslateProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fDeltaX = 1.0;
    fDeltaY = 1.0;
    fDeltaZ = 1.0;
    fAlpha = 0.;
    fBeta = 0.;
    fGamma = 0.;
}

void TRestDetectorHitsRotateAndTranslateProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fDeltaX = 1.0;
    fDeltaY = 1.0;
    fDeltaZ = 1.0;
    fAlpha = 0.;
    fBeta = 0.;
    fGamma = 0.;

    fInputHitsEvent = nullptr;
    fOutputHitsEvent = nullptr;
}

void TRestDetectorHitsRotateAndTranslateProcess::LoadConfig(string configFilename) {
    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();

    PrintMetadata();
}

void TRestDetectorHitsRotateAndTranslateProcess::InitProcess() {
    // Function to be executed once at the beginning of process
    // (before starting the process of the events)

    // Start by calling the InitProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::InitProcess();
}

TRestEvent* TRestDetectorHitsRotateAndTranslateProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)inputEvent;

    fOutputHitsEvent = fInputHitsEvent;
    // fInputHitsEvent->CloneTo(fOutputHitsEvent);

    TVector3 meanPosition = fOutputHitsEvent->GetMeanPosition();
    for (int hit = 0; hit < fOutputHitsEvent->GetNumberOfHits(); hit++) {
        fOutputHitsEvent->GetHits()->RotateIn3D(hit, fAlpha, fBeta, fGamma, meanPosition);
        fOutputHitsEvent->GetHits()->Translate(hit, fDeltaX, fDeltaY, fDeltaZ);
    }

    if (fOutputHitsEvent->GetNumberOfHits() == 0) return nullptr;

    debug << "Number of hits rotated: " << fInputHitsEvent->GetNumberOfHits() << endl;
    return fOutputHitsEvent;
}

void TRestDetectorHitsRotateAndTranslateProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

void TRestDetectorHitsRotateAndTranslateProcess::InitFromConfigFile() {
    fDeltaX = GetDblParameterWithUnits("deltaX");
    fDeltaY = GetDblParameterWithUnits("deltaY");
    fDeltaZ = GetDblParameterWithUnits("deltaZ");

    fAlpha = StringToDouble(GetParameter("alpha"));  // rotation angle around Z
    fBeta = StringToDouble(GetParameter("beta"));    // rotation angle around Y
    fGamma = StringToDouble(GetParameter("gamma"));  // rotation angle around X

    // Conversion to radians
    fAlpha = fAlpha * TMath::Pi() / 180.;
    fBeta = fBeta * TMath::Pi() / 180.;
    fGamma = fGamma * TMath::Pi() / 180.;
}
