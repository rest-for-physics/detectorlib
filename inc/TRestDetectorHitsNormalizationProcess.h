///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsNormalizationProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsNormalizationProcess
#define RestCore_TRestDetectorHitsNormalizationProcess

#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorHitsNormalizationProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

    Double_t fFactor;

   public:
    RESTValue GetInputEvent() const override { return fHitsInputEvent; }
    RESTValue GetOutputEvent() const override { return fHitsOutputEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " Renormalization factor : " << fFactor << RESTendl;

        EndPrintProcess();
    }

    inline TRestMetadata* GetProcessMetadata() const { return nullptr; }

    const char* GetProcessName() const override { return "hitsNormalizationProcess"; }

    TRestDetectorHitsNormalizationProcess();
    TRestDetectorHitsNormalizationProcess(const char* configFilename);

    ~TRestDetectorHitsNormalizationProcess();

    ClassDefOverride(TRestDetectorHitsNormalizationProcess, 1);
};
#endif
