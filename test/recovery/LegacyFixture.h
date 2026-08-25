#ifndef REST_LEGACY_RECOVERY_FIXTURE_H
#define REST_LEGACY_RECOVERY_FIXTURE_H

#include <TObject.h>
#include <TString.h>
#include <TTimeStamp.h>

#include <string>
#include <vector>

class TRestEvent : public TObject {
   public:
    Int_t fRunOrigin = 0;
    Int_t fSubRunOrigin = 0;
    Int_t fEventID = 0;
    Int_t fSubEventID = 0;
    TString fSubEventTag;
    TTimeStamp fEventTime;
    Bool_t fOk = true;
    ClassDef(TRestEvent, 1)
};

class TRestDetectorSignal {
   public:
    Int_t fSignalID = -1;
    std::vector<Float_t> fSignalTime;
    std::vector<Float_t> fSignalCharge;
    std::string fName;
    std::string fType;
    ClassDef(TRestDetectorSignal, 3)
};

class TRestDetectorSignalEvent : public TRestEvent {
   public:
    std::vector<TRestDetectorSignal> fSignal;
    ClassDef(TRestDetectorSignalEvent, 1)
};

class LegacyOpaqueEvent : public TObject {
   public:
    Int_t fCode = 0;
    std::vector<Float_t> fSamples;
    ClassDef(LegacyOpaqueEvent, 1)
};

class LegacyOpaqueMetadata : public TObject {
   public:
    TString fLabel;
    Int_t fValue = 0;
    ClassDef(LegacyOpaqueMetadata, 1)
};

#endif
