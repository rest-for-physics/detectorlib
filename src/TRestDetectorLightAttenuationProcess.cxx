//
// Created by lobis on 25-Aug-23.
//

#include "TRestDetectorLightAttenuationProcess.h"

using namespace std;

ClassImp(TRestDetectorLightAttenuationProcess);

void TRestDetectorLightAttenuationProcess::InitProcess() {
    if (fVetoLightAttenuationLength <= 0.0) {
        cerr << "TRestDetectorLightAttenuationProcess::InitProcess() - "
                "parameter 'vetoLightAttenuationLength' is not set. "
                "Please set it in the rml file."
             << endl;
        exit(1);
    }

    fReadout = GetMetadata<TRestDetectorReadout>();

    if (fReadout == nullptr) {
        cerr << "TRestDetectorLightAttenuationProcess::InitProcess() - "
                "TRestDetectorReadout is not defined. "
                "Please define it in the rml file."
             << endl;
        exit(1);
    }

    fInputEvent = nullptr;
    fOutputEvent = new TRestDetectorHitsEvent();
}

TRestEvent* TRestDetectorLightAttenuationProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = dynamic_cast<TRestDetectorHitsEvent*>(inputEvent);
    fOutputEvent->SetEventInfo(fInputEvent);

    for (unsigned int hit = 0; hit < fInputEvent->GetNumberOfHits(); hit++) {
        const TVector3& position = fInputEvent->GetPosition(hit);
        const REST_HitType hitType = fInputEvent->GetType(hit);
        const double energy = fInputEvent->GetEnergy(hit);
        const double time = fInputEvent->GetTime(hit);

        if (hitType != VETO) {
            // this process only works for veto hits
            fOutputEvent->AddHit(position, energy, time, hitType);
            continue;
        }

        // attenuation

        int count = 0;
        double energyAttenuated = energy;
        double timeDelayed = time;
        for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
            auto [daqId, moduleId, channelId] = fReadout->GetHitsDaqChannelAtReadoutPlane(position, p);
            TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlane(p);

            if (daqId >= 0) {
                auto channel = fReadout->GetReadoutChannelWithDaqID(daqId);

                const bool isVeto = (channel->GetChannelType() == "veto");

                if (!isVeto) {
                    cout << "TRestDetectorLightAttenuationProcess::ProcessEvent() - "
                            "channel type is not 'veto'. "
                         << endl;
                    continue;
                }

                plane->GetType();
                const double distance = plane->GetDistanceTo(position);
                energyAttenuated = energy * TMath::Exp(-1.0 * distance / fVetoLightAttenuationLength);
                timeDelayed = time + distance / fVetoEffectiveLightSpeed;
                count++;
            }
        }

        if (count == 0) {
            cerr << "TRestDetectorLightAttenuationProcess::ProcessEvent() - "
                    "no readout plane found for veto hit "
                 << position.X() << " " << position.Y() << " " << position.Z() << " with energy: " << energy
                 << " keV" << endl;
            exit(1);
            // it's probably a hit on the boundary, the readout should be slightly bigger to account for this
            // imprecision
        } else if (count > 1) {
            cerr << "TRestDetectorLightAttenuationProcess::ProcessEvent() - "
                    "more than one readout plane found for veto hit "
                 << endl;
            exit(1);
        }

        fOutputEvent->AddHit(position, energyAttenuated, timeDelayed, hitType);
    }

    return fOutputEvent;
}

void TRestDetectorLightAttenuationProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << "Veto effective light speed : " << fVetoEffectiveLightSpeed * units("m/s") << " m/s"
                 << RESTendl;
    RESTMetadata << "Veto light attenuation length : " << fVetoLightAttenuationLength * units("cm") << " cm"
                 << RESTendl;

    EndPrintProcess();
}
