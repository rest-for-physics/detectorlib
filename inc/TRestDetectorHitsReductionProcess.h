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
    inline any GetInputEvent() const { return fInputHitsEvent; }
    inline any GetOutputEvent() const { return fOutputHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();
    void LoadDefaultConfig();

    void LoadConfig(std::string configFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Starting distance : " << fStartingDistance << endl;
        metadata << " Minimum distance : " << fMinimumDistance << endl;
        metadata << " Distance step factor : " << fDistanceFactor << endl;
        metadata << " Maximum number of nodes : " << fMaxNodes << endl;

        EndPrintProcess();
    }

    inline const char* GetProcessName() const { return "hitsReduction"; }

    TRestDetectorHitsReductionProcess();
    TRestDetectorHitsReductionProcess(char* configFilename);

    ~TRestDetectorHitsReductionProcess();

    ClassDef(TRestDetectorHitsReductionProcess, 1);
};
#endif
