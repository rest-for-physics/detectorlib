///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorDaqChannelSwitchingProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorDaqChannelSwitchingProcess
#define RestCore_TRestDetectorDaqChannelSwitchingProcess

#include <TH1D.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorDaqChannelSwitchingProcess : public TRestEventProcess {
   private:
    TRestEvent* fEvent;              //!
    TRestDetectorReadout* fReadout;  //!

    std::map<int, int> fFirstDaqChannelDef;  //[module id, first daq id]
    bool fIgnoreUndefinedModules;

    void InitFromConfigFile() override;

    void Initialize() override;

   public:
    any GetInputEvent() const override { return fEvent; }
    any GetOutputEvent() const override { return fEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "module's daq channel re-definition: " << RESTendl;
        auto iter = fFirstDaqChannelDef.begin();
        while (iter != fFirstDaqChannelDef.end()) {
            RESTMetadata << "module id: " << iter->first << " first daq channel: " << iter->second << RESTendl;
            iter++;
        }
        RESTMetadata << RESTendl;

        EndPrintProcess();
    }

    TRestDetectorDaqChannelSwitchingProcess();
    ~TRestDetectorDaqChannelSwitchingProcess();

    ClassDefOverride(TRestDetectorDaqChannelSwitchingProcess, 1);
};
#endif
