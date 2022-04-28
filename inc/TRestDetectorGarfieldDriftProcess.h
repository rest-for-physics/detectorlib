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

#include "TRestDetectorGas.h"

#if defined USE_Garfield_OLD
#include "AvalancheMC.hh"
#include "ComponentBase.hh"
#include "Sensor.hh"
typedef AvalancheMC DRIFT_METHOD;
#elif USE_Garfield
#include "AvalancheMC.hh"
#include "Component.hh"
#include "Sensor.hh"
typedef AvalancheMC DRIFT_METHOD;
// TODO Could we have this as an option given through metadata?
#endif

#include <TRandom3.h>

#include "TRestDetectorGeometry.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"
#include "TRestEventProcess.h"

class TRestDetectorGarfieldDriftProcess : public TRestEventProcess {
   private:
    void Initialize() override;

    TRandom3* fRandom;

    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

#if defined USE_Garfield
    TRestDetectorReadout* fReadout;    //!
    TRestDetectorGas* fGas;            //!
    TRestDetectorGeometry* fGeometry;  //!

    Garfield::Sensor* fGfSensor;   //!
    DRIFT_METHOD* fGfDriftMethod;  //!

    void InitFromConfigFile() override;

    void LoadDefaultConfig();

    Int_t FindModule(Int_t readoutPlane, Double_t x, Double_t y);
    Int_t FindChannel(Int_t module, Double_t x, Double_t y);
#endif
   protected:
#if defined USE_Garfield
    Double_t fGasPressure;     // atm
                               //         Double_t fElectricField; // V/cm
    Double_t fDriftPotential;  // V
    Double_t fPEReduction;     // reduction factor of primary electrons to track
    double fStopDistance;      ///< Distance from readout to stop electron drift, in mm

    TString fGDML_Filename;

#endif
   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

#if defined USE_Garfield
    void InitProcess() override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        //             std::cout << "Electric field : " << fElectricField << " V/cm"
        //             << endl;
        metadata << "Drift electrode potential : " << fDriftPotential << " V" << endl;
        metadata << "Gas pressure : " << fGasPressure << " atm" << endl;
        metadata << "Electron reduction factor : " << fPEReduction << endl;
        metadata << "Drift stop distance : " << fStopDistance << " mm" << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() const { return fReadout; }

    const char* GetProcessName() const override { return "garfieldDrift"; }

    Garfield::Sensor* GetGfSensor() { return fGfSensor; }

    // Constructor
    TRestDetectorGarfieldDriftProcess();
    TRestDetectorGarfieldDriftProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorGarfieldDriftProcess();
#endif
    ClassDefOverride(TRestDetectorGarfieldDriftProcess,
             1);  // Template for a REST "event process" class inherited from
                  // TRestEventProcess
};
#endif
