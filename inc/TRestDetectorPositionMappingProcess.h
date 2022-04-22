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

    void InitFromConfigFile();

    void Initialize();
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
    inline any GetInputEvent() const { return fHitsEvent; }
    inline any GetOutputEvent() const { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    double GetCorrection2(double x, double y);
    double GetCorrection3(double x, double y, double z);

    void PrintMetadata() {
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

    TString GetProcessName() { return (TString) "readoutAnalysis"; }

    TRestDetectorPositionMappingProcess();
    TRestDetectorPositionMappingProcess(char* cfgFileName);

    ~TRestDetectorPositionMappingProcess();

    ClassDef(TRestDetectorPositionMappingProcess, 1);
};
#endif
