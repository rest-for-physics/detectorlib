///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestDetectorElectronDiffusionProcess.cxx
///
///
///             First implementation of electron diffusion process into REST_v2
///             Date : Sep/2015
///             Author : J. Galan
///
///_______________________________________________________________________________

#include "TRestDetectorElectronDiffusionProcess.h"

using namespace std;

ClassImp(TRestDetectorElectronDiffusionProcess);

TRestDetectorElectronDiffusionProcess::TRestDetectorElectronDiffusionProcess() { Initialize(); }

TRestDetectorElectronDiffusionProcess::TRestDetectorElectronDiffusionProcess(const char* configFilename) {
    Initialize();
    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }
}

TRestDetectorElectronDiffusionProcess::~TRestDetectorElectronDiffusionProcess() { delete fOutputHitsEvent; }

void TRestDetectorElectronDiffusionProcess::LoadDefaultConfig() {
    SetTitle("Default config");

    fElectricField = 2000;
    fAttachment = 0;
    fGasPressure = -1;
}

void TRestDetectorElectronDiffusionProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fElectricField = 0;
    fAttachment = 0;
    fGasPressure = -1;

    fTransversalDiffusionCoefficient = 0;
    fLongitudinalDiffusionCoefficient = 0;
    fWValue = 0;
    fFano = 0;

    fOutputHitsEvent = new TRestDetectorHitsEvent();
    fInputHitsEvent = nullptr;

    fGas = nullptr;
    fReadout = nullptr;

    fRandom = nullptr;
}

void TRestDetectorElectronDiffusionProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) {
        LoadDefaultConfig();
    }
}

void TRestDetectorElectronDiffusionProcess::InitProcess() {
    fRandom = new TRandom3(fSeed);

    fGas = GetMetadata<TRestDetectorGas>();
    if (fGas == nullptr) {
        if (fLongitudinalDiffusionCoefficient == -1 || fTransversalDiffusionCoefficient == -1) {
            RESTWarning << "Gas has not been initialized" << RESTendl;
            RESTError
                << "TRestDetectorElectronDiffusionProcess: diffusion parameters are not defined in the rml "
                   "file!"
                << RESTendl;
            exit(-1);
        }
        if (fWValue == -1) {
            RESTWarning << "Gas has not been initialized" << RESTendl;
            RESTError
                << "TRestDetectorElectronDiffusionProcess: gas work function has not been defined in the "
                   "rml file!"
                << RESTendl;
            exit(-1);
        }
    } else {
#ifndef USE_Garfield
        RESTError << "A TRestDetectorGas definition was found but REST was not linked to Garfield libraries."
                  << RESTendl;
        RESTError
            << "Please, remove the TRestDetectorGas definition, and add gas parameters inside the process "
               "TRestDetectorElectronDiffusionProcess"
            << RESTendl;
        exit(1);
#endif
        if (fGasPressure <= 0) fGasPressure = fGas->GetPressure();
        if (fElectricField <= 0) fElectricField = fGas->GetElectricField();
        if (fWValue <= 0) fWValue = fGas->GetWvalue(); 
        if (fFano <= 0) fFano = fGas->GetGasFanoFactor();

        fGas->SetPressure(fGasPressure);
        fGas->SetElectricField(fElectricField);
        fGas->SetW(fWValue);

        if (fLongitudinalDiffusionCoefficient <= 0) {
            fLongitudinalDiffusionCoefficient = fGas->GetLongitudinalDiffusion();
        }  // (cm)^1/2

        if (fTransversalDiffusionCoefficient <= 0) {
            fTransversalDiffusionCoefficient = fGas->GetTransversalDiffusion();
        }  // (cm)^1/2
    }

    fReadout = GetMetadata<TRestDetectorReadout>();
    if (fReadout == nullptr) {
        cout << "REST ERROR: Readout has not been initialized" << endl;
        exit(-1);
    }
}

TRestEvent* TRestDetectorElectronDiffusionProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputHitsEvent = (TRestDetectorHitsEvent*)inputEvent;
    fOutputHitsEvent->SetEventInfo(fInputHitsEvent);

    set<unsigned int> hitsToProcess;  // indices of the hits to process (we do not want to process veto hits)
    for (unsigned int n = 0; n < fInputHitsEvent->GetNumberOfHits(); n++) {
        if (fInputHitsEvent->GetType(n) == REST_HitType::VETO) {
            // keep unprocessed hits as they are
            fOutputHitsEvent->AddHit(fInputHitsEvent->GetX(n), fInputHitsEvent->GetY(n),
                                     fInputHitsEvent->GetZ(n), fInputHitsEvent->GetEnergy(n),
                                     fInputHitsEvent->GetTime(n), fInputHitsEvent->GetType(n));
        } else {
            hitsToProcess.insert(n);
        }
    }

    if (hitsToProcess.empty()) {
        return nullptr;
    }

    double totalEnergy = 0;
    for (const auto& hitIndex : hitsToProcess) {
        totalEnergy += fInputHitsEvent->GetEnergy(hitIndex);
    }

    bool isAttached;

    Double_t wValue = fWValue;
    const unsigned int totalElectrons = totalEnergy * REST_Units::eV / wValue;

    // TODO: double check this
    if (fMaxHits > 0 && totalElectrons > fMaxHits) {
        // set a fake w-value if max hits are limited. this fake w-value will be larger
        wValue = (totalEnergy * REST_Units::eV) / fMaxHits;
    }

    for (const auto& hitIndex : hitsToProcess) {
        TRestHits* hits = fInputHitsEvent->GetHits();

        const auto energy = hits->GetEnergy(hitIndex);
        const auto time = hits->GetTime(hitIndex);
        const auto type = hits->GetType(hitIndex);

        if (energy <= 0) {
            continue;
        }

        const Double_t x = hits->GetX(hitIndex);
        const Double_t y = hits->GetY(hitIndex);
        const Double_t z = hits->GetZ(hitIndex);

        for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
            TRestDetectorReadoutPlane* plane = &(*fReadout)[p];
            const auto planeType = plane->GetType();
            if (planeType == "veto") {
                // do not drift veto planes
                continue;
            }

            if (fCheckIsInside && !plane->IsInside({x, y, z})) {
                continue;
            }

            Double_t driftDistance = plane->GetDistanceTo({x, y, z});

            Int_t numberOfElectrons;
            Double_t fanofactor = fFano;
            if (fPoissonElectronExcitation) {
                numberOfElectrons = fRandom->Poisson(energy * fanofactor * REST_Units::eV / fWValue);
                if (wValue != fWValue) {
                    // reduce the number of electrons to improve speed
                    numberOfElectrons = numberOfElectrons * fWValue / wValue;
                }
            } else {
                numberOfElectrons = energy * REST_Units::eV / wValue;
            }

            if (numberOfElectrons <= 0) {
                numberOfElectrons = 1;
            }

            const Double_t energyPerElectron = energy * REST_Units::eV / numberOfElectrons;

            while (numberOfElectrons > 0) {
                numberOfElectrons--;

                Double_t longitudinalDiffusion =
                    10. * TMath::Sqrt(driftDistance / 10.) * fLongitudinalDiffusionCoefficient;  // mm
                Double_t transversalDiffusion =
                    10. * TMath::Sqrt(driftDistance / 10.) * fTransversalDiffusionCoefficient;  // mm

                if (fAttachment > 0) {
                    // TODO: where is this formula from?
                    isAttached = (fRandom->Uniform(0, 1) > pow(1 - fAttachment, driftDistance / 10.));
                } else {
                    isAttached = false;
                }

                if (isAttached) {
                    continue;
                }

                TVector3 positionAfterDiffusion = {x, y, z};
                positionAfterDiffusion += {
                    fRandom->Gaus(0, transversalDiffusion),  //
                    fRandom->Gaus(0, transversalDiffusion),  //
                    fRandom->Gaus(0, longitudinalDiffusion)  //
                };
                if (plane->GetDistanceTo(positionAfterDiffusion) < 0) {
                    // electron has been moved under the plane
                    positionAfterDiffusion.SetZ(
                        plane->GetPosition().Z() +
                        1E-6 * plane->GetNormal().Z());  // add a delta to make sure readout finds it
                }
                if (plane->GetDistanceTo(positionAfterDiffusion) > plane->GetHeight()) {
                    // electron has been moved over the plane
                    positionAfterDiffusion.SetZ(
                        plane->GetPosition().Z() + plane->GetHeight() -
                        1E-6 * plane->GetNormal().Z());  // add a delta to make sure readout finds it
                }

                if (!fCheckIsInside && !plane->IsInside(positionAfterDiffusion)) {
                    // electron has been moved outside the readout plane
                    continue;
                }

                const double electronEnergy =
                    fUnitElectronEnergy ? 1 : energyPerElectron * REST_Units::keV / REST_Units::eV;

                if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Extreme) {
                    cout << "Adding hit. x : " << positionAfterDiffusion.X()
                         << " y : " << positionAfterDiffusion.Y() << " z : " << positionAfterDiffusion.Z()
                         << " en : " << energyPerElectron * REST_Units::keV / REST_Units::eV << " keV"
                         << endl;
                }
                fOutputHitsEvent->AddHit(positionAfterDiffusion.X(), positionAfterDiffusion.Y(),
                                         positionAfterDiffusion.Z(), electronEnergy, time, type);
            }
        }
    }

    if (this->GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
        cout << "TRestDetectorElectronDiffusionProcess. Processed hits total energy : " << totalEnergy
             << endl;
        cout << "TRestDetectorElectronDiffusionProcess. Hits processed : " << hitsToProcess.size() << endl;
        cout << "TRestDetectorElectronDiffusionProcess. Hits added : " << fOutputHitsEvent->GetNumberOfHits()
             << endl;
        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Extreme) {
            GetChar();
        }
    }

    return fOutputHitsEvent;
}

void TRestDetectorElectronDiffusionProcess::EndProcess() {}

void TRestDetectorElectronDiffusionProcess::InitFromConfigFile() {
    fGasPressure = GetDblParameterWithUnits("gasPressure", -1.);
    fElectricField = GetDblParameterWithUnits("electricField", -1.);
    fWValue = GetDblParameterWithUnits("WValue", 0.0) * REST_Units::eV;
    fFano = GetDblParameterWithUnits("FanoFactor", 0.0);
    fAttachment = StringToDouble(GetParameter("attachment", "0"));
    fLongitudinalDiffusionCoefficient =
        StringToDouble(GetParameter("longitudinalDiffusionCoefficient", "-1"));
    if (fLongitudinalDiffusionCoefficient == -1)
        fLongitudinalDiffusionCoefficient = StringToDouble(GetParameter("longDiff", "-1"));
    else {
        RESTWarning << "longitudinalDiffusionCoefficient is now OBSOLETE! It will soon dissapear."
                    << RESTendl;
        RESTWarning << " Please use the shorter form of this parameter : longDiff" << RESTendl;
    }

    fTransversalDiffusionCoefficient = StringToDouble(GetParameter("transversalDiffusionCoefficient", "-1"));
    if (fTransversalDiffusionCoefficient == -1)
        fTransversalDiffusionCoefficient = StringToDouble(GetParameter("transDiff", "-1"));
    else {
        RESTWarning << "transversalDiffusionCoefficient is now OBSOLETE! It will soon dissapear." << RESTendl;
        RESTWarning << " Please use the shorter form of this parameter : transDiff" << RESTendl;
    }
    fMaxHits = StringToInteger(GetParameter("maxHits", "1000"));
    fSeed = StringToDouble(GetParameter("seed", "0"));
    fPoissonElectronExcitation = StringToBool(GetParameter("poissonElectronExcitation", "false"));
    fUnitElectronEnergy = StringToBool(GetParameter("unitElectronEnergy", "false"));
    fCheckIsInside = StringToBool(GetParameter("checkIsInside", "true"));
}
