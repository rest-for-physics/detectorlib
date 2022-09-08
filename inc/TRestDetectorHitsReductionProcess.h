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

    void InitFromConfigFile() override;

    void Initialize() override;

   protected:
    Double_t fStartingDistance;
    Double_t fMinimumDistance;
    Double_t fDistanceFactor;
    Double_t fMaxNodes;

   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;
    void LoadDefaultConfig();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " Starting distance : " << fStartingDistance << RESTendl;
        RESTMetadata << " Minimum distance : " << fMinimumDistance << RESTendl;
        RESTMetadata << " Distance step factor : " << fDistanceFactor << RESTendl;
        RESTMetadata << " Maximum number of nodes : " << fMaxNodes << RESTendl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "hitsReduction"; }

    TRestDetectorHitsReductionProcess();
    TRestDetectorHitsReductionProcess(const char* configFilename);

    ~TRestDetectorHitsReductionProcess();

    ClassDefOverride(TRestDetectorHitsReductionProcess, 1);
};
#endif
