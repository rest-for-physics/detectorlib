///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsSmearingProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsSmearingProcess
#define RestCore_TRestDetectorHitsSmearingProcess

#include <TRandom3.h>
#include <TRestEventProcess.h>

#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"

class TRestDetectorHitsSmearingProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

    TRandom3* fRandom;  //!

    TRestDetectorGas* fGas;  //!

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

    Double_t fEnergyRef;         ///< reference energy for the FWHM
    Double_t fResolutionAtERef;  ///< FWHM at Energy of reference

   public:
    inline any GetInputEvent() const { return fHitsInputEvent; }
    inline any GetOutputEvent() const { return fHitsOutputEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " reference energy (ERef): " << fEnergyRef << endl;
        metadata << " resolution at ERef : " << fResolutionAtERef << endl;

        EndPrintProcess();
    }

    inline TRestMetadata* GetProcessMetadata() const { return nullptr; }

    inline const char* GetProcessName() const { return "smearingProcess"; }

    inline Double_t GetEnergyReference() const { return fEnergyRef; }
    inline Double_t GetResolutionReference() const { return fResolutionAtERef; }

    TRestDetectorHitsSmearingProcess();
    TRestDetectorHitsSmearingProcess(char* configFilename);

    ~TRestDetectorHitsSmearingProcess();

    ClassDef(TRestDetectorHitsSmearingProcess, 2);
};
#endif
