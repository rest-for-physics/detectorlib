///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             Process which create ionization electrons and drift them toward
///             the readout plane (stop 1mm above)
///
///             TRestDetectorGarfieldDriftProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorGarfieldDriftProcess
#define RestCore_TRestDetectorGarfieldDriftProcess

#include <TRestDetectorGas.h>

#if defined REST_GARFIELD
#include "AvalancheMC.hh"
#include "Component.hh"
#include "Sensor.hh"
typedef AvalancheMC DRIFT_METHOD;
#endif

#include <TRandom3.h>

#include "TRestDetectorGeometry.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"
#include "TRestEventProcess.h"

class TRestDetectorGarfieldDriftProcess : public TRestEventProcess {
   private:
    void Initialize();

    TRandom3* fRandom;

    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

#if defined REST_GARFIELD
    TRestDetectorReadout* fReadout;    //!
    TRestDetectorGas* fGas;            //!
    TRestDetectorGeometry* fGeometry;  //!

    Garfield::Sensor* fGfSensor;   //!
    DRIFT_METHOD* fGfDriftMethod;  //!

    void InitFromConfigFile();

    void LoadDefaultConfig();

    Int_t FindModule(Int_t readoutPlane, Double_t x, Double_t y);
    Int_t FindChannel(Int_t module, Double_t x, Double_t y);

   protected:
    Double_t fGasPressure;     // atm
                               //         Double_t fElectricField; // V/cm
    Double_t fDriftPotential;  // V
    Double_t fPEReduction;     // reduction factor of primary electrons to track
    double fStopDistance;      ///< Distance from readout to stop electron drift, in mm

    TString fGDML_Filename;

#endif
   public:
    any GetInputEvent() { return fInputHitsEvent; }
    any GetOutputEvent() { return fOutputHitsEvent; }

    TRestEvent* ProcessEvent(TRestEvent* eventInput);

#if defined REST_GARFIELD
    void InitProcess();
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        //             std::cout << "Electric field : " << fElectricField << " V/cm"
        //             << endl;
        metadata << "Drift electrode potential : " << fDriftPotential << " V" << endl;
        metadata << "Gas pressure : " << fGasPressure << " atm" << endl;
        metadata << "Electron reduction factor : " << fPEReduction << endl;
        metadata << "Drift stop distance : " << fStopDistance << " mm" << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() { return fReadout; }

    TString GetProcessName() { return (TString) "garfieldDrift"; }

    Garfield::Sensor* GetGfSensor() { return fGfSensor; }

    // Constructor
    TRestDetectorGarfieldDriftProcess();
    TRestDetectorGarfieldDriftProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorGarfieldDriftProcess();
#endif
    ClassDef(TRestDetectorGarfieldDriftProcess, 1);
};

#endif
