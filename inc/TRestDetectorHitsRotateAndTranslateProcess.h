///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorHitsRotateAndTranslateProcess.h
///
///             march 2016:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier G. Garza
///_______________________________________________________________________________

#ifndef RestCore_TRestDetectorHitsRotateAndTranslateProcess
#define RestCore_TRestDetectorHitsRotateAndTranslateProcess

#include <TRestEventProcess.h>

#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"

class TRestDetectorHitsRotateAndTranslateProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    Double_t fDeltaX;  ///< shift in X-axis
    Double_t fDeltaY;  ///< shift in X-axis
    Double_t fDeltaZ;  ///< shift in X-axis

    Double_t fAlpha;  ///< rotation angle around z-axis
    Double_t fBeta;   ///< rotation angle around y-axis
    Double_t fGamma;  ///< rotation angle around x-axis

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(std::string configFilename);

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " delta x : " << fDeltaX << RESTendl;
        RESTMetadata << " delta y : " << fDeltaY << RESTendl;
        RESTMetadata << " delta z : " << fDeltaZ << RESTendl;
        RESTMetadata << " alpha : " << fAlpha << RESTendl;
        RESTMetadata << " beta : " << fBeta << RESTendl;
        RESTMetadata << " gamma : " << fGamma << RESTendl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "rotateAndTraslate"; }

    inline Double_t GetDeltaX() const { return fDeltaX; }
    inline Double_t GetDeltaY() const { return fDeltaY; }
    inline Double_t GetDeltaZ() const { return fDeltaZ; }

    inline Double_t GetAlpha() const { return fAlpha; }
    inline Double_t GetBeta() const { return fBeta; }
    inline Double_t GetGamma() const { return fGamma; }

    // Constructor
    TRestDetectorHitsRotateAndTranslateProcess();
    TRestDetectorHitsRotateAndTranslateProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorHitsRotateAndTranslateProcess();

    ClassDefOverride(TRestDetectorHitsRotateAndTranslateProcess,
                     1);  // Template for a REST "event process" class inherited from
                          // TRestEventProcess
};
#endif
