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

//! A process to transform a x,y,z coordinate hits into daq identified physical time signals
class TRestDetectorHitsToSignalProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorHitsEvent output
    TRestDetectorHitsEvent* fHitsEvent;  //!

    /// A pointer to the specific TRestDetectorHitsEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A pointer to the detector readout definition accesible to TRestRun
    TRestDetectorReadout* fReadout;  //!

    /// A pointer to the detector gas definition accessible to TRestRun
    TRestDetectorGas* fGas;  //!

    void Initialize();

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
    inline any GetInputEvent() const { return fHitsEvent; }
    inline any GetOutputEvent() const { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Sampling : " << fSampling << " us" << endl;
        metadata << "Electric field : " << fElectricField * units("V/cm") << " V/cm" << endl;
        metadata << "Gas pressure : " << fGasPressure << " atm" << endl;
        metadata << "Drift velocity : " << fDriftVelocity << " mm/us" << endl;

        EndPrintProcess();
    }

    TRestMetadata* GetProcessMetadata() const { return fReadout; }

    /// Returns the name of this process
    inline const char* GetProcessName() const { return "hitsToSignal"; }

    TRestDetectorHitsToSignalProcess();
    TRestDetectorHitsToSignalProcess(char* configFilename);
    ~TRestDetectorHitsToSignalProcess();

    ClassDef(TRestDetectorHitsToSignalProcess, 1);
};
#endif
