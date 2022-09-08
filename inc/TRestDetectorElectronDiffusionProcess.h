///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorElectronDiffusionProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorElectronDiffusionProcess
#define RestCore_TRestDetectorElectronDiffusionProcess

#include <TRandom3.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>

#include "TRestEventProcess.h"

class TRestDetectorElectronDiffusionProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    TRestDetectorGas* fGas;          //!
    TRestDetectorReadout* fReadout;  //!

    TRandom3* fRandom;  //!
#endif

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    Double_t fElectricField;
    Double_t fAttachment;
    Double_t fGasPressure;
    Double_t fWvalue;
    Double_t fLonglDiffCoeff;
    Double_t fTransDiffCoeff;
    Bool_t fPoissonElectronExcitation;
    Bool_t fUnitElectronEnergy;

    Int_t fMaxHits;

    Double_t fSeed = 0;

   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " eField : " << fElectricField * units("V/cm") << " V/cm" << RESTendl;
        RESTMetadata << " attachment coeficient : " << fAttachment << " V/cm" << RESTendl;
        RESTMetadata << " gas pressure : " << fGasPressure << " atm" << RESTendl;
        RESTMetadata << " longitudinal diffusion coefficient : " << fLonglDiffCoeff << " cm^1/2" << RESTendl;
        RESTMetadata << " transversal diffusion coefficient : " << fTransDiffCoeff << " cm^1/2" << RESTendl;
        RESTMetadata << " W value : " << fWvalue << " eV" << RESTendl;

        RESTMetadata << " Maximum number of hits : " << fMaxHits << RESTendl;

        RESTMetadata << " seed : " << fSeed << RESTendl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() const { return fGas; }

    const char* GetProcessName() const override { return "electronDiffusion"; }

    inline Double_t GetElectricField() const { return fElectricField; }
    inline Double_t GetAttachmentCoefficient() const { return fAttachment; }
    inline Double_t GetGasPressure() const { return fGasPressure; }

    // Constructor
    TRestDetectorElectronDiffusionProcess();
    TRestDetectorElectronDiffusionProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorElectronDiffusionProcess();

    ClassDefOverride(TRestDetectorElectronDiffusionProcess, 3);  // Template for a REST "event process" class
                                                                 // inherited from TRestEventProcess
};
#endif
