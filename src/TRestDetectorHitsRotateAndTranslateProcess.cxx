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

void TRestDetectorHitsRotateAndTranslateProcess::LoadDefaultConfig() { SetTitle("Default config"); }

void TRestDetectorHitsRotateAndTranslateProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputHitsEvent = nullptr;
    fOutputHitsEvent = nullptr;

    // Get volume name from parameter
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

    if (fOutputHitsEvent->GetNumberOfHits() == 0) {
        return nullptr;
    }

    for (unsigned int hit = 0; hit < fOutputHitsEvent->GetNumberOfHits(); hit++) {
        const auto& type = fOutputHitsEvent->GetHits()->GetType(hit);
        if (type == VETO) {
            // Do not rotate VETO hits (we typically only rotate TPC hits)
            continue;
        }
        fOutputHitsEvent->GetHits()->RotateIn3D(hit, fRotationVector.X(), fRotationVector.Y(),
                                                fRotationVector.Z(), fRotationCenter);
        fOutputHitsEvent->GetHits()->Translate(hit, fTranslationVector.X(), fTranslationVector.Y(),
                                               fTranslationVector.Z());
    }

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
