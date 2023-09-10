///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSignalViewerProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorSignalViewerProcess
#define RestCore_TRestDetectorSignalViewerProcess

#include <TH1D.h>
#include <TRestEventProcess.h>

#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"

class TRestDetectorSignalViewerProcess : public TRestEventProcess {
   private:
    TRestDetectorSignalEvent* fSignalEvent;  //!

    std::vector<TObject*> fDrawingObjects;  //!
    Double_t fDrawRefresh;                  //!

    TVector2 fBaseLineRange;  //!

    int eveCounter = 0;  //!
    int sgnCounter = 0;  //!

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    RESTValue GetInputEvent() const override { return fSignalEvent; }
    RESTValue GetOutputEvent() const override { return fSignalEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        std::cout << "Refresh value : " << fDrawRefresh << std::endl;

        EndPrintProcess();
    }

    TPad* DrawSignal(Int_t signal);

    const char* GetProcessName() const override { return "rawSignalViewer"; }

    TRestDetectorSignalViewerProcess();
    TRestDetectorSignalViewerProcess(const char* configFilename);

    ~TRestDetectorSignalViewerProcess();

    ClassDefOverride(TRestDetectorSignalViewerProcess, 1);
};
#endif
