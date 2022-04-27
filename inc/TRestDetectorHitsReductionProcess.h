//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsReductionProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorHitsReductionProcess
#define RestCore_TRestDetectorHitsReductionProcess

#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsReductionProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    void InitFromConfigFile();

    void Initialize();

   protected:
    Double_t fStartingDistance;
    Double_t fMinimumDistance;
    Double_t fDistanceFactor;
    Double_t fMaxNodes;

   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* inputEvent);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Starting distance : " << fStartingDistance << endl;
        metadata << " Minimum distance : " << fMinimumDistance << endl;
        metadata << " Distance step factor : " << fDistanceFactor << endl;
        metadata << " Maximum number of nodes : " << fMaxNodes << endl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "hitsReduction"; }

    TRestDetectorHitsReductionProcess();
    TRestDetectorHitsReductionProcess(const char* configFilename);

    ~TRestDetectorHitsReductionProcess();

    ClassDef(TRestDetectorHitsReductionProcess, 1);
};
#endif
