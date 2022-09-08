///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSingleChannelAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorSingleChannelAnalysisProcess
#define RestCore_TRestDetectorSingleChannelAnalysisProcess

#include <TH1D.h>
#include <TRestEventProcess.h>

#include "TRestDetectorGainMap.h"
#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"

class TRestDetectorSingleChannelAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorSignalEvent* fSignalEvent;  //!

    TRestDetectorReadout* fReadout;  //!

    TRestDetectorGainMap* fCalib;  //!
#endif

    void InitFromConfigFile() override;

    void Initialize() override;
    // parameters
    bool fApplyGainCorrection;
    bool fCreateGainMap;
    TVector2 fThrIntegralCutRange;
    TVector2 fNGoodSignalsCutRange;
    TVector2 fSpecFitRange;
    std::string fCalibSave;

    // analysis result
    std::map<int, TH1D*> fChannelThrIntegral;  //-> [channel id, sum]
    std::map<int, double> fChannelFitMean;     // [MM id, fitted position]
    std::map<int, double> fChannelGain;        // [MM id, channel gain]
    std::map<int, double> fChannelGainError;   // [MM id, channel gain error]

   public:
    any GetInputEvent() const override { return fSignalEvent; }
    any GetOutputEvent() const override { return fSignalEvent; }

    void FitChannelGain();
    // See comments on CXX
    void SaveGainMetadata(std::string filename);
    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;
    TH1D* GetChannelSpectrum(int id);
    void PrintChannelSpectrums(std::string filename);

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "the mode is:" << RESTendl;
        RESTMetadata << (fApplyGainCorrection ? ">   " : "    ")
                 << "Apply channel correction std::map for spectrum " << RESTendl;
        RESTMetadata << (fCreateGainMap ? ">   " : "    ") << "Create new correction std::map for each channel"
                 << RESTendl;
        RESTMetadata << "output mapping file: " << fCalibSave << RESTendl;
        RESTMetadata << "Energy cut for Threshold integral: " << any(fThrIntegralCutRange) << RESTendl;
        RESTMetadata << "Energy cut for NGoodSignals: " << any(fNGoodSignalsCutRange) << RESTendl;
        RESTMetadata << "Fit range for the spectra: " << any(fSpecFitRange) << RESTendl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "readoutAnalysis"; }

    // Constructor
    TRestDetectorSingleChannelAnalysisProcess();
    TRestDetectorSingleChannelAnalysisProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorSingleChannelAnalysisProcess();

    ClassDefOverride(TRestDetectorSingleChannelAnalysisProcess, 1);
};
#endif
