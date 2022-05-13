///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorPositionMappingProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorPositionMappingProcess
#define RestCore_TRestDetectorPositionMappingProcess

#include <TH1D.h>
#include <TRestEventProcess.h>

#include "TRestDetectorGainMap.h"
#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"

class TRestDetectorPositionMappingProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fHitsEvent;  //!
    TRestDetectorReadout* fReadout;      //!
    TRestDetectorGainMap* fCalib;        //!
    TRestDetectorGas* fGas;              //!

    void InitFromConfigFile() override;

    void Initialize() override;
    // parameters
    bool fApplyGainCorrection;
    bool fCreateGainMap;
    TVector2 fEnergyCutRange;
    TVector2 fNHitsCutRange;
    std::string fMappingSave;

    double fNBinsX;
    double fNBinsY;
    double fNBinsZ;

    // temp data
    TH2D* fAreaThrIntegralSum;  //!
    TH2D* fAreaCounts;          //!

    // analysis result
    TH2F* fAreaGainMap;  //!

   public:
    any GetInputEvent() const override { return fHitsEvent; }
    any GetOutputEvent() const override { return fHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    double GetCorrection2(double x, double y);
    double GetCorrection3(double x, double y, double z);

    void PrintMetadata() override {
        BeginPrintProcess();

        metadata << "the mode is:" << endl;
        metadata << (fApplyGainCorrection ? ">   " : "    ")
                 << "Apply position correction std::map for spectrum " << endl;
        metadata << (fCreateGainMap ? ">   " : "    ") << "Create new correction std::map for each position"
                 << endl;
        metadata << "output mapping file: " << fMappingSave << endl;
        metadata << "Energy cut for Threshold integral: " << any(fEnergyCutRange) << endl;
        metadata << "Energy cut for NGoodSignals: " << any(fNHitsCutRange) << endl;
        metadata << "Binning: " << fNBinsX << ", " << fNBinsY << ", " << fNBinsZ << endl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "readoutAnalysis"; }

    TRestDetectorPositionMappingProcess();
    TRestDetectorPositionMappingProcess(const char* configFilename);

    ~TRestDetectorPositionMappingProcess();

    ClassDefOverride(TRestDetectorPositionMappingProcess, 1);
};
#endif
