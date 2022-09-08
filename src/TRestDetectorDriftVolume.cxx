
#include "TRestDetectorDriftVolume.h"

using namespace REST_Units;
using namespace std;

ClassImp(TRestDetectorDriftVolume);

TRestDetectorDriftVolume::TRestDetectorDriftVolume() { Initialize(); }
TRestDetectorDriftVolume::TRestDetectorDriftVolume(const char* configFilename, string name)
    : TRestMetadata(configFilename) {
    LoadConfigFromFile(configFilename, name);
}

void TRestDetectorDriftVolume::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fMaterial = "";
    fW = -1;

    fElectricField = 0;
    fDriftVelocity = 0;
    fElectronLifeTime = 0;
    fLongitudinalDiffusion = 0;
    fTransversalDiffusion = 0;
    fTownsendCoefficient = 0;
    fAttachmentCoefficient = 0;

    fPressureInAtm = 1;
    fTemperatureInK = 300;
}

void TRestDetectorDriftVolume::InitFromConfigFile() {
    fMaterial = GetParameter("material", "");
    fW = StringToDouble(GetParameter("W_value", "-1"));

    fElectricField = GetDblParameterWithUnits("electricField", 0.);
    fDriftVelocity = GetDblParameterWithUnits("driftVelocity", 0.);
    fElectronLifeTime = GetDblParameterWithUnits("electronLifeTime", 0.);
    fLongitudinalDiffusion = GetDblParameterWithUnits("longitudinalDiffusion", 0.);
    fTransversalDiffusion = GetDblParameterWithUnits("transversalDiffusion", 0.);
    fTownsendCoefficient = GetDblParameterWithUnits("townsendCoefficient", 0.);
    fAttachmentCoefficient = GetDblParameterWithUnits("attachmentCoefficient", 0.);

    fPressureInAtm = GetDblParameterWithUnits("pressure");
    fTemperatureInK = GetDblParameterWithUnits("temperature");
}

void TRestDetectorDriftVolume::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Drift Volume Material : " << fMaterial << ", W-value : " << fW << " eV" << RESTendl;
    RESTMetadata << "Electric Field : " << fElectricField * V / cm << " V/cm " << RESTendl;
    RESTMetadata << "Drift Speed : " << fDriftVelocity * mm / us << " mm/us" << RESTendl;
    RESTMetadata << "Electron LifeTime : " << fElectronLifeTime * ns << " ns" << RESTendl;
    RESTMetadata << "Longitudinal Diffusion : " << fLongitudinalDiffusion << " (cm)^1/2" << RESTendl;
    RESTMetadata << "Transversal Diffusion : " << fTransversalDiffusion << " (cm)^1/2" << RESTendl;
    RESTMetadata << "******************************************" << RESTendl;
    RESTMetadata << RESTendl;
    RESTMetadata << RESTendl;
}
