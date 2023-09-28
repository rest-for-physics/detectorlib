//
// Created by lobis on 03-Sep-23.
//

#ifndef REST_TRESTDETECTORHITSREADOUTANALYSISPROCESS_H
#define REST_TRESTDETECTORHITSREADOUTANALYSISPROCESS_H

#include <TCanvas.h>
#include <TH1D.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>
#include <TRestDetectorSignalEvent.h>
#include <TRestEventProcess.h>

//! An analysis REST process to extract valuable information from Hits type of data.
class TRestDetectorHitsReadoutAnalysisProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig(){};

    /// \brief This process will only work on hits corresponding to this channel type (using readout)
    std::string fChannelType;
    TVector3 fFiducialPosition;
    Double_t fFiducialDiameter = 0;
    bool fRemoveZeroEnergyEvents = false;

    TRestDetectorReadout* fReadout = nullptr;  //!

   public:
    RESTValue GetInputEvent() const override { return fInputHitsEvent; }
    RESTValue GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void PrintMetadata() override;

    const char* GetProcessName() const override { return "readoutHitsAnalysis"; }

    TRestDetectorHitsReadoutAnalysisProcess() = default;
    TRestDetectorHitsReadoutAnalysisProcess(const char* configFilename) {}

    ~TRestDetectorHitsReadoutAnalysisProcess() override = default;

    ClassDefOverride(TRestDetectorHitsReadoutAnalysisProcess, 2);
};

#endif  // REST_TRESTDETECTORHITSREADOUTANALYSISPROCESS_H
