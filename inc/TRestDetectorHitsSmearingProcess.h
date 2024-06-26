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
#include "TRestDetectorReadout.h"

/// A process to include detector energy resolution in a TRestDetectorHitsEvent
class TRestDetectorHitsSmearingProcess : public TRestEventProcess {
   private:
    /// A pointer to the process input event
    TRestDetectorHitsEvent* fInputEvent = nullptr;  //!

    /// A pointer to the process output event
    TRestDetectorHitsEvent* fOutputEvent = nullptr;  //!

    /// A pointer to the random generator initializes with fSeed
    TRandom3* fRandom = nullptr;  //!

    void Initialize() override;
    void InitProcess() override;

    std::string fChannelType = "tpc";

    TRestDetectorReadout* fReadout = nullptr;  //!

   protected:
    /// Reference energy for the FWHM
    Double_t fEnergyRef = 5.9;  //<

    /// FWHM at Energy of reference
    Double_t fResolutionAtERef = 15;  //<

    /// The seed to be used for the random generator
    ULong_t fSeed = 0;  //<

   public:
    RESTValue GetInputEvent() const override { return fInputEvent; }
    RESTValue GetOutputEvent() const override { return fOutputEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Channel type: " << fChannelType << RESTendl;

        RESTMetadata << "Reference energy (ERef): " << fEnergyRef << RESTendl;
        RESTMetadata << "Resolution at ERef : " << fResolutionAtERef << RESTendl;

        EndPrintProcess();
    }

    inline TRestMetadata* GetProcessMetadata() const { return nullptr; }

    const char* GetProcessName() const override { return "smearingProcess"; }

    /// Returns the reference energy where the FWHM is defined
    inline Double_t GetEnergyReference() const { return fEnergyRef; }

    /// Returns the energy resolution in %FWHM for the reference energy
    inline Double_t GetResolutionReference() const { return fResolutionAtERef; }

    TRestDetectorHitsSmearingProcess();
    TRestDetectorHitsSmearingProcess(const char* configFilename);

    ~TRestDetectorHitsSmearingProcess();

    ClassDefOverride(TRestDetectorHitsSmearingProcess, 4);
};
#endif
