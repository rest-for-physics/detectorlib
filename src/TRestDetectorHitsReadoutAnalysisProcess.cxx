//
// Created by lobis on 03-Sep-23.
//

#include "TRestDetectorHitsReadoutAnalysisProcess.h"

using namespace std;

TRestEvent* TRestDetectorHitsReadoutAnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    fHitsEvent = dynamic_cast<TRestDetectorHitsEvent*>(inputEvent);

    return fHitsEvent;
}

void TRestDetectorHitsReadoutAnalysisProcess::InitProcess() {
    fReadout = dynamic_cast<TRestDetectorReadout*>(fRunInfo->GetMetadataClass("TRestDetectorReadout"));
    if (!fReadout) {
        cerr << "TRestDetectorHitsReadoutAnalysisProcess::InitProcess() : "
             << "TRestDetectorReadout not found" << endl;
        exit(1);
    }

    cout << "TRestDetectorHitsReadoutAnalysisProcess::InitProcess() : "
         << "TRestDetectorReadout found" << endl;
}

void TRestDetectorHitsReadoutAnalysisProcess::EndProcess() {}

void TRestDetectorHitsReadoutAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << "Channel type : " << fChannelType << RESTendl;
    RESTMetadata << "Fiducial position : (" << fFiducialPosition.X() << " , " << fFiducialPosition.Y()
                 << " , " << fFiducialPosition.Z() << ")" << RESTendl;
    RESTMetadata << "Fiducial diameter : " << fFiducialDiameter << RESTendl;

    EndPrintProcess();
}
