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

TRestDetectorHitsRotateAndTranslateProcess::~TRestDetectorHitsRotateAndTranslateProcess() = default;

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

void TRestDetectorHitsRotateAndTranslateProcess::LoadConfig(const string& configFilename) {
    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }

    PrintMetadata();
}

void TRestDetectorHitsRotateAndTranslateProcess::InitProcess() {}

TRestEvent* TRestDetectorHitsRotateAndTranslateProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)inputEvent;

    fOutputHitsEvent = fInputHitsEvent;

    TVector3 meanPosition = fOutputHitsEvent->GetMeanPosition();
    for (unsigned int hit = 0; hit < fOutputHitsEvent->GetNumberOfHits(); hit++) {
        fOutputHitsEvent->GetHits()->RotateIn3D(hit, fAlpha, fBeta, fGamma, meanPosition);
        fOutputHitsEvent->GetHits()->Translate(hit, fDeltaX, fDeltaY, fDeltaZ);
    }

    if (fOutputHitsEvent->GetNumberOfHits() == 0) return nullptr;

    RESTDebug << "Number of hits rotated: " << fInputHitsEvent->GetNumberOfHits() << RESTendl;
    return fOutputHitsEvent;
}

void TRestDetectorHitsRotateAndTranslateProcess::EndProcess() {}

void TRestDetectorHitsRotateAndTranslateProcess::InitFromConfigFile() {
    fRotationCenter = Get3DVectorParameterWithUnits("rotationCenter", fRotationCenter);
    fTranslationVector = Get3DVectorParameterWithUnits("translation", fTranslationVector);

    // rotation units should be specified in the rml (e.g. "90deg")
    double rotationX = GetDblParameterWithUnits("rotationX", fRotationVector.X());
    double rotationY = GetDblParameterWithUnits("rotationY", fRotationVector.Y());
    double rotationZ = GetDblParameterWithUnits("rotationZ", fRotationVector.Z());
    fRotationVector = {rotationX, rotationY, rotationZ};

    // legacy (maybe deprecated soon)
    if (fTranslationVector.Mag2() == 0) {
        fTranslationVector = {
            GetDblParameterWithUnits("deltaX", 0),  //
            GetDblParameterWithUnits("deltaY", 0),  //
            GetDblParameterWithUnits("deltaZ", 0),  //
        };
    }
    if (fRotationVector.Mag2() == 0) {
        fRotationVector = {
            GetDblParameterWithUnits("alpha", 0),  //
            GetDblParameterWithUnits("beta", 0),   //
            GetDblParameterWithUnits("gamma", 0),  //
        };
    }
}
