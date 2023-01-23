/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see https://gifna.unizar.es/trex                 *
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
 * If not, see https://www.gnu.org/licenses/.                            *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestDetectorHitsSmearingProcess
#define RestCore_TRestDetectorHitsSmearingProcess

#include <TRandom3.h>
#include <TRestEventProcess.h>

#include "TRestDetectorHitsEvent.h"

/// A process to include detector energy resolution in a TRestDetectorHitsEvent
class TRestDetectorHitsSmearingProcess : public TRestEventProcess {
   private:
    /// A pointer to the process input event
    TRestDetectorHitsEvent* fInputEvent = nullptr;  //!

    /// A pointer to the process output event
    TRestDetectorHitsEvent* fOutputEvent = nullptr;  //!

    /// A pointer to the random generator initialized by fSeed
    TRandom3* fRandom = nullptr;  //!

    void Initialize() override;

   protected:
    /// Energy of reference where we will apply fResolutionReference
    Double_t fEnergyReference = 5.9;  //<

    /// FWHM in % at the energy of reference
    Double_t fResolutionReference = 15;  //<

    /// A seed for the random generator
    Double_t fSeed = 0;  //<

   public:
    any GetInputEvent() const override { return fInputEvent; }
    any GetOutputEvent() const override { return fOutputEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Reference energy (ERef) in keV: " << fEnergyReference << RESTendl;
        RESTMetadata << "FWHM resolution at ERef in \%: " << fResolutionReference << RESTendl;

        EndPrintProcess();
    }

    /// Returns the name of the process
    const char* GetProcessName() const override { return "smearingProcess"; }

    /// Returns the reference energy where the FWHM is defined
    inline Double_t GetEnergyReference() const { return fEnergyReference; }

    /// Returns the energy resolution in %FWHM for the reference energy
    inline Double_t GetResolutionReference() const { return fResolutionReference; }

    TRestDetectorHitsSmearingProcess();
    ~TRestDetectorHitsSmearingProcess();

    ClassDefOverride(TRestDetectorHitsSmearingProcess, 3);
};
#endif
