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

    TVector3 fTranslationVector = {0, 0, 0};  ///< translation vector (x,y,z)
    TVector3 fRotationVector = {0, 0, 0};     ///< rotation vector around axis (x,y,z) in radians
    TVector3 fRotationCenter = {0, 0, 0};     ///< rotation center (x,y,z)

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

    void LoadConfig(const std::string& configFilename);

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Translation vector (mm): (" << fTranslationVector.X() << ", "
                     << fTranslationVector.Y() << ", " << fTranslationVector.Z() << ")" << RESTendl;
        TVector3 rotationVectorDegrees = fRotationVector * (180. / TMath::Pi());
        RESTMetadata << "Rotation vector (degrees): (" << rotationVectorDegrees.X() << ", "
                     << rotationVectorDegrees.Y() << ", " << rotationVectorDegrees.Z() << ")" << RESTendl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "rotateAndTranslate"; }

    inline Double_t GetTranslationX() const { return fTranslationVector.X(); }
    inline Double_t GetTranslationY() const { return fTranslationVector.Y(); }
    inline Double_t GetTranslationZ() const { return fTranslationVector.Z(); }

    inline Double_t GetRotationX() const { return fRotationVector.X(); }
    inline Double_t GetRotationY() const { return fRotationVector.Y(); }
    inline Double_t GetRotationZ() const { return fRotationVector.Z(); }

    TRestDetectorHitsRotateAndTranslateProcess();
    explicit TRestDetectorHitsRotateAndTranslateProcess(const char* configFilename);
    ~TRestDetectorHitsRotateAndTranslateProcess();

    ClassDefOverride(TRestDetectorHitsRotateAndTranslateProcess, 2);
};

#endif
