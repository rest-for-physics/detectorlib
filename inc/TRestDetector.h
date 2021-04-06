////////////////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetector.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetector
#define RestCore_TRestDetector

#include <Rtypes.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TRestMetadata.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"

using namespace std;

class TRestDetector : public TRestMetadata {
   public:
    string fDetectorName = "REST Detector";
    Int_t fRunNumber = 0;

    Double_t fDriftVoltage = -1;   // In V
    Double_t fDriftDistance = -1;  // in mm
    Double_t fDriftGain = -1;      // in mm

    Double_t fAmplificationVoltage = -1;   // In V
    Double_t fAmplificationDistance = -1;  // in mm
    Double_t fAmplificationGain = -1;      //

    Double_t fMass = -1;    // in kg
    Double_t fHeight = -1;  // in mm
    Double_t fRadius = -1;  // in mm
    Double_t fLength = -1;  // in mm
    Double_t fWidth = -1;   // in mm

    Double_t fDAQShapingTime = -1;   // in us
    Double_t fDAQSamplingTime = -1;  // in us
    Double_t fDAQDynamicRange = -1;
    Double_t fDAQThreshold = -1;

    void InitFromConfigFile() { ReadAllParameters(); }
    void PrintMetadata();

    // Constructors
    TRestDetector() {}
    // Destructor
    ~TRestDetector() {}

    ClassDef(TRestDetector, 1);
};

#endif
