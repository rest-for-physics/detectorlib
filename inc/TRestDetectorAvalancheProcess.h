///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorAvalancheProcess.h
///
///             feb 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorAvalancheProcess
#define RestCore_TRestDetectorAvalancheProcess

#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

class TRestDetectorAvalancheProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

    Double_t fEnergyRef;         ///< reference energy for the FWHM
    Double_t fResolutionAtEref;  ///< FWHM at Energy of reference
    Double_t fDetectorGain;      ///< Detector's gain.

    void InitFromConfigFile();
    void Initialize();
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    TRestDetectorGas* fGas;  //!

   public:
    any GetInputEvent() const override { return fHitsInputEvent; }
    any GetOutputEvent() const override { return fHitsOutputEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* inputEvent);
    void EndProcess();

    void LoadConfig(std::string configFilename);

    void PrintMetadata() {
        BeginPrintProcess();

        metadata << " reference energy (Eref): " << fEnergyRef << endl;
        metadata << " resolution at Eref : " << fResolutionAtEref << endl;
        metadata << " detector gain : " << fDetectorGain << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() const { return fGas; }

    const char* GetProcessName() const override { return "avalancheProcess"; }

    inline Double_t GetEnergyReference() const { return fEnergyRef; }
    inline Double_t GetResolutionReference() const { return fResolutionAtEref; }
    inline Double_t GetDetectorGain() const { return fDetectorGain; }

    // Constructor
    TRestDetectorAvalancheProcess();
    TRestDetectorAvalancheProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorAvalancheProcess();

    ClassDef(TRestDetectorAvalancheProcess, 1);  // Template for a REST "event process"
                                                 // class inherited from TRestEventProcess
};
#endif
