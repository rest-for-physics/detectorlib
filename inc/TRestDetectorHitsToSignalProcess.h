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

#ifndef RestCore_TRestDetectorHitsToSignalProcess
#define RestCore_TRestDetectorHitsToSignalProcess

#include <TRestEventProcess.h>

#include "TRestDetectorGas.h"
#include "TRestDetectorHitsEvent.h"
#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"
#include "TRestPhysics.h"

//! A process to transform a x,y,z coordinate hits into daq identified physical time signals
class TRestDetectorHitsToSignalProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorHitsEvent output
    TRestDetectorHitsEvent* fHitsEvent;  //!

    /// A pointer to the specific TRestDetectorHitsEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A pointer to the detector readout definition available to TRestRun
    TRestDetectorReadout* fReadout;  //!

    /// A pointer to the detector gas definition accessible to TRestRun
    TRestDetectorGas* fGas;  //!

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    /// The sampling rate in us
    Double_t fSampling = 1;  //<

    /// The gas pressure. If defined it will change the TRestDetectorGas pressure in atm.
    Double_t fGasPressure = 1;  // atm

    /// The electric field in V/mm. Used to calculate the drift velocity if TRestDetectorGas is defined.
    Double_t fElectricField = 100;  //<

    /// The drift velocity in mm/us. If it is negative, it will be calculated from TRestDetectorGas.
    Double_t fDriftVelocity = -1;  // mm/us

   public:
    RESTValue GetInputEvent() const override { return fHitsEvent; }
    RESTValue GetOutputEvent() const override { return fSignalEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Sampling : " << fSampling << " us" << RESTendl;
        RESTMetadata << "Electric field : " << fElectricField * units("V/cm") << " V/cm" << RESTendl;
        RESTMetadata << "Gas pressure : " << fGasPressure << " atm" << RESTendl;
        RESTMetadata << "Drift velocity : " << fDriftVelocity << " mm/us" << RESTendl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() const { return fReadout; }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "hitsToSignal"; }

    TRestDetectorHitsToSignalProcess();
    TRestDetectorHitsToSignalProcess(const char* configFilename);
    ~TRestDetectorHitsToSignalProcess();

    ClassDefOverride(TRestDetectorHitsToSignalProcess, 3);
};
#endif
