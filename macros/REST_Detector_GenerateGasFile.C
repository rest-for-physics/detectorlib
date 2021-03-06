#include "TRestDetectorGas.h"
#include "TRestTask.h"

#ifndef RESTTask_GenerateGasFile
#define RESTTask_GenerateGasFile

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Detector_GenerateGasFile(char* cfgFile) {
    TRestDetectorGas* gas = new TRestDetectorGas(cfgFile, "", true);

    //   gas->SetGasPressure( 1. );

    gas->PlotDriftVelocity(1, 1.e7, 50);
    gas->PlotLongitudinalDiffusion(1, 5000, 50);
    gas->PlotTransversalDiffusion(1, 5000, 50);
    gas->PlotTownsendCoefficient(1, 5000, 50);

    return 0;
}
#endif
