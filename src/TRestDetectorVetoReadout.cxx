#include "TRestDetectorVetoReadout.h"

using namespace std;

ClassImp(TRestDetectorVetoReadout);

TRestDetectorVetoReadout::TRestDetectorVetoReadout(const char* configFilename, const char* name)
    : TRestDetectorVetoReadout() {
    // we call the base constructor instead of "Initialize" using constructor delegation
    fConfigFileName = configFilename;
    LoadConfigFromFile(fConfigFileName, name);
    cout << "TRestDetectorVetoReadout::TRestDetectorVetoReadout: " << endl;
}

TRestDetectorVetoReadout::TRestDetectorVetoReadout() {
    cout << "TRestDetectorVetoReadout::TRestDetectorVetoReadout: base" << endl;

    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);
}

void TRestDetectorVetoReadout::PrintMetadata() { cout << "Geometry: " << fGeometry << endl; }
