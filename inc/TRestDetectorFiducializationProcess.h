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

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string configFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "fiducialization"; }

    TRestDetectorFiducializationProcess();
    TRestDetectorFiducializationProcess(const char* configFilename);

    ~TRestDetectorFiducializationProcess();

    ClassDef(TRestDetectorFiducializationProcess, 1);
};
#endif
