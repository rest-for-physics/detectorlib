///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorFiducializationProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorFiducializationProcess
#define RestCore_TRestDetectorFiducializationProcess

#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>

#include "TRestEventProcess.h"

class TRestDetectorFiducializationProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    TRestDetectorReadout* fReadout;  //!

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "fiducialization"; }

    TRestDetectorFiducializationProcess();
    TRestDetectorFiducializationProcess(const char* configFilename);

    ~TRestDetectorFiducializationProcess();

    ClassDefOverride(TRestDetectorFiducializationProcess, 1);
};
#endif
