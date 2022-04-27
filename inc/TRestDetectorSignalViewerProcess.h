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

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fSignalEvent; }
    any GetOutputEvent() const override { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        std::cout << "Refresh value : " << fDrawRefresh << endl;

        EndPrintProcess();
    }

    TPad* DrawSignal(Int_t signal);

    const char* GetProcessName() const override { return "rawSignalViewer"; }

    TRestDetectorSignalViewerProcess();
    TRestDetectorSignalViewerProcess(const char* configFilename);

    ~TRestDetectorSignalViewerProcess();

    ClassDef(TRestDetectorSignalViewerProcess, 1);
};
#endif
