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

    void InitFromConfigFile();

    void Initialize();
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
    inline any GetInputEvent() const { return fSignalEvent; }
    inline any GetOutputEvent() const { return fSignalEvent; }

    void FitChannelGain();
    // See comments on CXX
    void SaveGainMetadata(std::string filename);
    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    TH1D* GetChannelSpectrum(int id);
    void PrintChannelSpectrums(std::string filename);

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "the mode is:" << endl;
        metadata << (fApplyGainCorrection ? ">   " : "    ")
                 << "Apply channel correction std::map for spectrum " << endl;
        metadata << (fCreateGainMap ? ">   " : "    ") << "Create new correction std::map for each channel"
                 << endl;
        metadata << "output mapping file: " << fCalibSave << endl;
        metadata << "Energy cut for Threshold integral: " << any(fThrIntegralCutRange) << endl;
        metadata << "Energy cut for NGoodSignals: " << any(fNGoodSignalsCutRange) << endl;
        metadata << "Fit range for the spectra: " << any(fSpecFitRange) << endl;

        EndPrintProcess();
    }

    inline const char* GetProcessName() const { return "readoutAnalysis"; }

    // Constructor
    TRestDetectorSingleChannelAnalysisProcess();
    TRestDetectorSingleChannelAnalysisProcess(char* configFilename);
    // Destructor
    ~TRestDetectorSingleChannelAnalysisProcess();

    ClassDef(TRestDetectorSingleChannelAnalysisProcess, 1);
};
#endif
