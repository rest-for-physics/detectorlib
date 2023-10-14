//
// Created by lobis on 03-Sep-23.
//

#include "TRestDetectorHitsReadoutAnalysisProcess.h"

#include <numeric>

using namespace std;

TRestEvent* TRestDetectorHitsReadoutAnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)inputEvent;

    vector<TVector3> hitPosition;
    vector<double> hitEnergy;
    double energyInFiducial = 0;

    for (size_t hitIndex = 0; hitIndex < fInputHitsEvent->GetNumberOfHits(); hitIndex++) {
        const auto position = fInputHitsEvent->GetPosition(hitIndex);
        const auto energy = fInputHitsEvent->GetEnergy(hitIndex);
        const auto time = fInputHitsEvent->GetTime(hitIndex);
        const auto type = fInputHitsEvent->GetType(hitIndex);

        if (energy <= 0) {
            // this should never happen
            cerr << "TRestDetectorHitsReadoutAnalysisProcess::ProcessEvent() : "
                 << "Negative or zero energy found in hit " << hitIndex << endl;
            exit(1);
        }

        const auto daqId = fReadout->GetDaqId(position, false);
        const auto channelType = fReadout->GetTypeForChannelDaqId(daqId);
        const bool isValidHit = channelType == fChannelType;

        const auto nHits = fOutputHitsEvent->GetNumberOfHits();
        if (isValidHit || !fRemoveHitsOutsideReadout) {
            fOutputHitsEvent->AddHit(position, energy, time, type);
        }
        if (!isValidHit) {
            // this hit is either not on the readout or the channel is not of the type we want
            continue;
        }
        if (fOutputHitsEvent->GetNumberOfHits() != nHits + 1) {
            // this should never happen
            cerr << "TRestDetectorHitsReadoutAnalysisProcess::ProcessEvent() : "
                 << "Error adding hit " << hitIndex << " to output event" << endl;
            exit(1);
        }

        hitPosition.push_back(position);
        hitEnergy.push_back(energy);

        if (fFiducialDiameter > 0) {
            TVector3 relativePosition = position - fFiducialPosition;
            relativePosition.SetZ(0);  // TODO: this should be relative to readout normal
            const double distance = relativePosition.Mag();
            if (distance > fFiducialDiameter / 2) {
                continue;
            }
            energyInFiducial += energy;
        }
    }

    const double readoutEnergy = accumulate(hitEnergy.begin(), hitEnergy.end(), 0.0);

    if (fRemoveZeroEnergyEvents && readoutEnergy <= 0) {
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

    SetObservableValue("readoutEnergyInFiducial", energyInFiducial);

    return fOutputHitsEvent;
}

void TRestDetectorHitsReadoutAnalysisProcess::InitProcess() {
    fReadout = dynamic_cast<TRestDetectorReadout*>(fRunInfo->GetMetadataClass("TRestDetectorReadout"));
    if (!fReadout) {
        cerr << "TRestDetectorHitsReadoutAnalysisProcess::InitProcess() : "
             << "TRestDetectorReadout not found" << endl;
        exit(1);
    }

    if (fChannelType.empty()) {
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
    fRemoveZeroEnergyEvents = StringToBool(GetParameter("removeZeroEnergyEvents", "false"));
    fRemoveHitsOutsideReadout = StringToBool(GetParameter("removeHitsOutsideReadout", "false"));
    fChannelType = GetParameter("channelType", fChannelType);
    fFiducialPosition = Get3DVectorParameterWithUnits("fiducialPosition", fFiducialPosition);
    fFiducialDiameter = GetDblParameterWithUnits("fiducialDiameter", fFiducialDiameter);
}

void TRestDetectorHitsReadoutAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputHitsEvent = nullptr;
    fOutputHitsEvent = new TRestDetectorHitsEvent();
}
