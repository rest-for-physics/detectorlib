///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorTriggerAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorTriggerAnalysisProcess
#define RestCore_TRestDetectorTriggerAnalysisProcess

#include <TH1D.h>

#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorTriggerAnalysisProcess : public TRestEventProcess {
   private:
    TRestDetectorSignalEvent* fSignalEvent;  //!

    std::vector<std::string> fIntegralObservables;  //!
    std::vector<double> fThreshold;                 //!

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    Double_t fW;
    Double_t fSampling;
    Int_t fADCLength;

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " W : " << fW << endl;

        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "triggerAnalysis"; }

    TRestDetectorTriggerAnalysisProcess();
    TRestDetectorTriggerAnalysisProcess(char* cfgFileName);

    ~TRestDetectorTriggerAnalysisProcess();

    ClassDef(TRestDetectorTriggerAnalysisProcess, 1);
};
#endif
