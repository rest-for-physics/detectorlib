////////////////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorSetup.h
///
///             Metadata class to be used to store basic detector setup info
///             inherited from TRestMetadata
///
///             jun 2016:   First concept. Javier Galan
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestDetectorSetup
#define RestCore_TRestDetectorSetup

#include <TRestMetadata.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

class TRestDetectorSetup : public TRestMetadata {
   private:
    void Initialize() override;

    void InitFromConfigFile() override;

    Int_t fRunNumber;
    Int_t fSubRunNumber;
    TString fRunTag;

    Double_t fMeshVoltage;
    Double_t fDriftField;        // Drift field in V/cm/bar
    Double_t fDetectorPressure;  // In bar

    TString fElectronicsGain;
    TString fShapingTime;
    TString fSamplingTime;

    Double_t fSamplingInMicroSec;

   public:
    inline Int_t GetRunNumber() const { return fRunNumber; }
    inline Int_t GetSubRunNumber() const { return fSubRunNumber; }
    inline TString GetRunTag() const { return fRunTag; }

    void SetRunNumber(Int_t runNumber) { fRunNumber = runNumber; }
    void SetSubRunNumber(Int_t subRunNumber) { fSubRunNumber = subRunNumber; }

    inline Double_t GetSamplingInMicroSeconds() const { return fSamplingInMicroSec; }

    Double_t GetFieldInVPerCm() { return fDriftField * fDetectorPressure; }

    inline Double_t GetPressureInBar() const { return fDetectorPressure; }

    void InitFromFileName(TString fName);

    void PrintMetadata() override;

    // Constructors
    TRestDetectorSetup();
    TRestDetectorSetup(const char* configFilename, const std::string& name = "");

    // Destructor
    ~TRestDetectorSetup();

    ClassDefOverride(TRestDetectorSetup, 1);
};
#endif
