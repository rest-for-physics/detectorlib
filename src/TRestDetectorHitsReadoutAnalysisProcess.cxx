//
// Created by lobis on 03-Sep-23.
//

#include "TRestDetectorHitsReadoutAnalysisProcess.h"

#include <numeric>

using namespace std;

TRestEvent* TRestDetectorHitsReadoutAnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    fHitsEvent = dynamic_cast<TRestDetectorHitsEvent*>(inputEvent);

    vector<TVector3> hitPosition;
    vector<double> hitEnergy;

    for (size_t hit_index = 0; hit_index < fHitsEvent->GetNumberOfHits(); hit_index++) {
        const auto position = fHitsEvent->GetPosition(hit_index);
        const auto energy = fHitsEvent->GetEnergy(hit_index);
        // const auto time = fHitsEvent->GetTime(hit_index);

        if (energy <= 0) {
            // this should never happen
            cerr << "TRestDetectorHitsReadoutAnalysisProcess::ProcessEvent() : "
                 << "Negative or zero energy found in hit " << hit_index << endl;
            exit(1);
        }

        const auto daqId = fReadout->GetDaqId(position);
        const auto channelType = fReadout->GetTypeForChannelDaqId(daqId);

        if (channelType != fChannelType) {
            continue;
        }

        hitPosition.push_back(position);
        hitEnergy.push_back(energy);
    }

    const double readoutEnergy = accumulate(hitEnergy.begin(), hitEnergy.end(), 0.0);

    if (readoutEnergy <= 0) {
        return nullptr;
    }

    TVector3 positionAverage;
    for (size_t i = 0; i < hitPosition.size(); i++) {
        positionAverage += hitPosition[i] * hitEnergy[i];
    }
    positionAverage *= 1.0 / readoutEnergy;

    // standard deviation weighted with energy
    TVector3 positionSigma;
    for (size_t i = 0; i < hitPosition.size(); i++) {
        TVector3 diff2 = hitPosition[i] - positionAverage;
        diff2.SetX(diff2.X() * diff2.X());
        diff2.SetY(diff2.Y() * diff2.Y());
        diff2.SetZ(diff2.Z() * diff2.Z());
        positionSigma += diff2 * hitEnergy[i];
    }
    positionSigma *= 1.0 / readoutEnergy;
    positionSigma.SetX(sqrt(positionSigma.X()));
    positionSigma.SetY(sqrt(positionSigma.Y()));
    positionSigma.SetZ(sqrt(positionSigma.Z()));

    SetObservableValue("readoutEnergy", readoutEnergy);
    SetObservableValue("readoutNumberOfHits", hitEnergy.size());

    SetObservableValue("readoutAveragePositionX", positionAverage.X());
    SetObservableValue("readoutAveragePositionY", positionAverage.Y());
    SetObservableValue("readoutAveragePositionZ", positionAverage.Z());

    SetObservableValue("readoutSigmaPositionX", positionSigma.X());
    SetObservableValue("readoutSigmaPositionY", positionSigma.Y());
    SetObservableValue("readoutSigmaPositionZ", positionSigma.Z());

    return fHitsEvent;
}

void TRestDetectorHitsReadoutAnalysisProcess::InitProcess() {
    fReadout = dynamic_cast<TRestDetectorReadout*>(fRunInfo->GetMetadataClass("TRestDetectorReadout"));
    if (!fReadout) {
        cerr << "TRestDetectorHitsReadoutAnalysisProcess::InitProcess() : "
             << "TRestDetectorReadout not found" << endl;
        exit(1);
    }

    if (fChannelType == "") {
        cerr << "TRestDetectorHitsReadoutAnalysisProcess::InitProcess() : "
             << "Channel type not defined" << endl;
        exit(1);
    }
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

void TRestDetectorHitsReadoutAnalysisProcess::InitFromConfigFile() {
    fChannelType = GetParameter("channelType", fChannelType);
    fFiducialPosition = Get3DVectorParameterWithUnits("fiducialPosition", fFiducialPosition);
    fFiducialDiameter = GetDblParameterWithUnits("fiducialDiameter", fFiducialDiameter);
}
