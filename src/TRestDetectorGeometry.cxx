///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorGeometry.cxx
///
///             Base geometry class used to store a geometry written by GDML or
///             define a new geometry using TGeoManager class
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestDetectorGeometry.h"

using namespace std;

ClassImp(TRestDetectorGeometry);

TRestDetectorGeometry::TRestDetectorGeometry() : TGeoManager() {}

TRestDetectorGeometry::~TRestDetectorGeometry() {
#if defined REST_GARFIELD
    for (auto item : vReadoutElec) {
        delete item;
    }
    vReadoutElec.clear();

    for (auto item : vGfSensor) {
        delete item;
    }
    vGfSensor.clear();

    for (auto item : vGfComponent) {
        delete item;
    }
    vGfComponent.clear();
#endif
}

void TRestDetectorGeometry::InitGfGeometry() {
#if defined REST_GARFIELD
    fGfGeometry = new Garfield::GeometryRoot();
    fGfGeometry->SetGeometry(this);
#endif
}

void TRestDetectorGeometry::PrintGeometry() {
    cout << "--------------------------------------" << endl;
    cout << "Volumes : " << this->GetListOfVolumes()->GetEntries() << endl;
    cout << "Mother volume : " << this->GetTopVolume()->GetName() << endl;
    cout << "--------------------------------------" << endl;

    TGeoVolume* vol;
    TGeoMaterial* mat;
    for (int volID = 0; volID < this->GetListOfVolumes()->GetEntries(); volID++) {
        vol = this->GetVolume(volID);
        mat = vol->GetMaterial();
        cout << "Volume " << volID << endl;
        cout << "       Name : " << vol->GetName() << endl;
        cout << "       Material : " << mat->GetName() << endl;
        cout << "       Density : " << mat->GetDensity() << endl;
    }
}
