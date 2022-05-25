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

#ifndef RestCore_TRestDetectorSignalToHitsProcess
#define RestCore_TRestDetectorSignalToHitsProcess

#include <TRestEventProcess.h>

#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"

//! A process to transform a daq channel and physical time to spatial coordinates
class TRestDetectorSignalToHitsProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorHitsEvent output
    TRestDetectorHitsEvent* fHitsEvent;  //!

    /// A pointer to the specific TRestDetectorHitsEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A pointer to the detector readout definition accesible to TRestRun
    TRestDetectorReadout* fReadout;  //!

    /// A pointer to the detector gas definition accessible to TRestRun
    TRestDetectorGas* fGas;  //!

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    /// The electric field in standard REST units (V/mm). Only relevant if TRestDetectorGas is used.
    Double_t fElectricField = 100;

    /// The gas pressure in atm. Only relevant if TRestDetectorGas is used.
    Double_t fGasPressure = 1;

    /// The drift velocity in standard REST units (mm/us).
    Double_t fDriftVelocity = -1;

    /// The method used to transform the signal points to hits.
    TString fMethod = "tripleMax";

    // Time window to integrate in case intwindow method is requested, units (us)
    Double_t fIntWindow = 5;

    // Threshold value for in case intwindow method is requested
    Double_t fThreshold = 100.;

   public:
    any GetInputEvent() const override { return fSignalEvent; }
    any GetOutputEvent() const override { return fHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Electric field : " << fElectricField * units("V/cm") << " V/cm" << RESTendl;
        RESTMetadata << "Gas pressure : " << fGasPressure << " atm" << RESTendl;
        RESTMetadata << "Drift velocity : " << fDriftVelocity << " mm/us" << RESTendl;
        RESTMetadata << "Signal to hits method : " << fMethod << RESTendl;
        if (fMethod == "intwindow") {
            RESTMetadata << "Threshold : " << fThreshold << " ADC" << RESTendl;
            RESTMetadata << "Integral window : " << fIntWindow << " us" << RESTendl;
        }

        EndPrintProcess();
    }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "signalToHits"; }

    TRestDetectorSignalToHitsProcess();
    TRestDetectorSignalToHitsProcess(const char* configFilename);
    ~TRestDetectorSignalToHitsProcess();

    ClassDefOverride(TRestDetectorSignalToHitsProcess, 4);
};
#endif
