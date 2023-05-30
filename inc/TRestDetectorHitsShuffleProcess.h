//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsShuffleProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorHitsShuffleProcess
#define RestCore_TRestDetectorHitsShuffleProcess

#include <TRandom3.h>
#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsShuffleProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fHitsEvent;  //!
    TRandom3* fRandom;  //!

    void InitFromConfigFile() override;

    void Initialize() override;

   protected:
    Int_t fIterations;

   public:
    any GetInputEvent() const override { return fHitsEvent; }
    any GetOutputEvent() const override { return fHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;
    void LoadDefaultConfig();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " Iterations : " << fIterations << RESTendl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "hitsShuffle"; }

    // Constructor
    TRestDetectorHitsShuffleProcess();
    TRestDetectorHitsShuffleProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorHitsShuffleProcess();

    ClassDefOverride(TRestDetectorHitsShuffleProcess, 1);  // Template for a REST "event process" class
                                                           // inherited from TRestEventProcess
};
#endif
