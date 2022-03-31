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

#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

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

    void Initialize();

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

   public:
    any GetInputEvent() { return fSignalEvent; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Electric field : " << fElectricField * units("V/cm") << " V/cm" << endl;
        metadata << "Gas pressure : " << fGasPressure << " atm" << endl;
        metadata << "Drift velocity : " << fDriftVelocity << " mm/us" << endl;

        metadata << "Signal to hits method : " << fMethod << endl;

        EndPrintProcess();
    }

    /// Returns the name of this process
    TString GetProcessName() { return (TString) "signalToHits"; }

    TRestDetectorSignalToHitsProcess();
    TRestDetectorSignalToHitsProcess(char* cfgFileName);
    ~TRestDetectorSignalToHitsProcess();

    ClassDef(TRestDetectorSignalToHitsProcess, 3);
};
#endif
