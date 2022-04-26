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

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

    Double_t fFactor;

   public:
    any GetInputEvent() { return fHitsInputEvent; }
    any GetOutputEvent() { return fHitsOutputEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string configFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " Renormalization factor : " << fFactor << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() const { return NULL; }

    inline TString GetProcessName() const { return (TString) "hitsNormalizationProcess"; }

    TRestDetectorHitsNormalizationProcess();
    TRestDetectorHitsNormalizationProcess(char* configFilename);

    ~TRestDetectorHitsNormalizationProcess();

    ClassDef(TRestDetectorHitsNormalizationProcess, 1);
};
#endif
