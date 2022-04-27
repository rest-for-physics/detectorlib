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
#ifndef __CINT__
    TRestDetectorHitsEvent* fHitsEvent;  //!

    TRandom3* fRandom;  //!
#endif

    void InitFromConfigFile();

    void Initialize();

   protected:
    Int_t fIterations;

   public:
    any GetInputEvent() const override { return fHitsEvent; }
    any GetOutputEvent() const override { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Iterations : " << fIterations << endl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "hitsShuffle"; }

    // Constructor
    TRestDetectorHitsShuffleProcess();
    TRestDetectorHitsShuffleProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorHitsShuffleProcess();

    ClassDef(TRestDetectorHitsShuffleProcess, 1);  // Template for a REST "event process" class inherited from
                                                   // TRestEventProcess
};
#endif
