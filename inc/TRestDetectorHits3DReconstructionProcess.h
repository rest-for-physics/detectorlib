//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHits3DReconstructionProcess.h
///
///              Nov 2016 : Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorHits3DReconstructionProcess
#define RestCore_TRestDetectorHits3DReconstructionProcess

#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHits3DReconstructionProcess : public TRestEventProcess {
   private:
    // input/output event with concrete type
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    // unsaved parameters, temporary data members
    TH2D* htemp = nullptr;                      //!
    TRestEventProcess* fCompareProc = nullptr;  //!

    // process parameters
    Double_t fZRange;
    bool fDraw;
    double fDrawThres;
    bool fDoEnergyScaling;

   protected:
    void InitFromConfigFile() override;
    void Initialize() override;

   public:
    RESTValue GetInputEvent() const override { return fInputHitsEvent; }
    RESTValue GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    double LogAmbiguity(const int& n, const int& m) { return log(Ambiguity(n, m)); }
    int Ambiguity(const int& n, const int& m);
    int Factorial(const int& n);

    // Process Information Printer
    void PrintMetadata() override;
    // Constructor
    TRestDetectorHits3DReconstructionProcess();
    // Destructor
    ~TRestDetectorHits3DReconstructionProcess();

    ClassDefOverride(TRestDetectorHits3DReconstructionProcess, 1);
};
#endif
