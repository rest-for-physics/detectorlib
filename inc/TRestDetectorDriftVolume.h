
/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestDetectorDriftVolume
#define RestCore_TRestDetectorDriftVolume

#include <TROOT.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

#include "TApplication.h"
#include "TArrayI.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TNamed.h"
#include "TRestMetadata.h"
#include "TString.h"
#include "TSystem.h"
#include "TVector3.h"

class TRestDetectorDriftVolume : public TRestMetadata {
   protected:
    std::string fMaterial;  // material description std::string
    Double_t fW;            // Work function for electron extraction, in unit eV.

    Double_t fElectricField;          // in unit V/mm
    Double_t fDriftVelocity;          // in unit mm/us
    Double_t fElectronLifeTime;       // in unit us
    Double_t fLongitudinalDiffusion;  // in unit (cm)^1/2
    Double_t fTransversalDiffusion;   // in unit (cm)^1/2
    Double_t fTownsendCoefficient;
    Double_t fAttachmentCoefficient;

    Double_t fPressureInAtm;   // Pressure of the gas in atm.
    Double_t fTemperatureInK;  // Temperature of the gas in K.

   public:
    TRestDetectorDriftVolume();
    TRestDetectorDriftVolume(const char* configFilename, std::string name = "");
    virtual void Initialize();
    virtual void InitFromConfigFile();

    virtual std::string GetMaterial() const { return fMaterial; }
    virtual Double_t GetW() const { return fW; }
    virtual Double_t GetWvalue() const { return fW; }

    /// Returns the electric field in V/mm.
    virtual Double_t GetElectricField() const { return fElectricField; }

    /// Returns the drift velocity in mm/us.
    virtual Double_t GetDriftVelocity() const { return fDriftVelocity; }

    virtual Double_t GetElectronLifeTime() const { return fElectronLifeTime; }
    virtual Double_t GetLongitudinalDiffusion() const { return fLongitudinalDiffusion; }
    virtual Double_t GetTransversalDiffusion() const { return fTransversalDiffusion; }
    virtual Double_t GetTownsendCoefficient() const { return fTownsendCoefficient; }
    virtual Double_t GetAttachmentCoefficient() const { return fAttachmentCoefficient; }

    virtual Double_t GetPressure() const { return fPressureInAtm; }
    virtual Double_t GetTemperature() const { return fTemperatureInK; }

    virtual void SetMaterial(std::string value) { fMaterial = value; }

    /// Sets the electric field of the drift volume. Given in V/mm.
    virtual void SetW(double value) { fW = value; }

    /// Sets the electric field. Must be given in V/mm.
    virtual void SetElectricField(double value) { fElectricField = value; }

    // Sets the drift velocity. Must be given in mm/us.
    virtual void SetDriftVelocity(double value) { fDriftVelocity = value; }

    virtual void SetElectronLifeTime(double value) { fElectronLifeTime = value; }
    virtual void SetLongitudinalDiffusion(double value) { fLongitudinalDiffusion = value; }
    virtual void SetTransversalDiffusion(double value) { fTransversalDiffusion = value; }
    virtual void SetTownsendCoefficient(double value) { fTownsendCoefficient = value; }
    virtual void SetAttachmentCoefficient(double value) { fAttachmentCoefficient = value; }

    virtual void SetPressure(double value) { fPressureInAtm = value; }
    virtual void SetTemperature(double value) { fTemperatureInK = value; }

    virtual void UpdateCondition() {}

    virtual void PrintMetadata();

    ClassDef(TRestDetectorDriftVolume, 1);  // Gas Parameters
};

#endif
