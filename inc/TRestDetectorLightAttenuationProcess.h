//
// Created by lobis on 25-Aug-23.
//

#ifndef REST_TRESTDETECTORLIGHTATTENUATIONPROCESS_H
#define REST_TRESTDETECTORLIGHTATTENUATIONPROCESS_H

#include <TRandom3.h>
#include <TRestEventProcess.h>

#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"

/// A process to include detector energy resolution in a TRestDetectorHitsEvent
class TRestDetectorLightAttenuationProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fInputEvent;   //!
    TRestDetectorHitsEvent* fOutputEvent;  //!

    TRestDetectorReadout* fReadout;  //!

    /// Length of the light attenuation in the veto. One length reduces the signal amplitude to 1/e of the
    /// original
    Double_t fVetoLightAttenuationLength = 0.0;  // mm
    /// Effective light speed for veto signals. Used to compute travel time from the interaction point to the
    /// veto readout.
    Double_t fVetoEffectiveLightSpeed = REST_Physics::lightSpeed;  // mm/us

    void Initialize() override {}
    void LoadDefaultConfig() {}

   public:
    RESTValue GetInputEvent() const override { return fInputEvent; }
    RESTValue GetOutputEvent() const override { return fOutputEvent; }

    void InitProcess() override;
    void EndProcess() override {}

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    TRestDetectorLightAttenuationProcess() = default;
    explicit TRestDetectorLightAttenuationProcess(const char* configFilename){};

    const char* GetProcessName() const override { return "lightAttenuation"; }

    ~TRestDetectorLightAttenuationProcess() override = default;

    ClassDefOverride(TRestDetectorLightAttenuationProcess, 1);
};

#endif  // REST_TRESTDETECTORLIGHTATTENUATIONPROCESS_H
