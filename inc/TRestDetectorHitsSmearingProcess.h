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
    TRestDetectorHitsEvent* fHitsInputEvent;   //!
    TRestDetectorHitsEvent* fHitsOutputEvent;  //!

    TRandom3* fRandom;  //!

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();

   protected:
    /// Reference energy for the FWHM
    Double_t fEnergyRef = 5.9;  //<

    /// FWHM at Energy of reference
    Double_t fResolutionAtERef = 15;  //<

   public:
    any GetInputEvent() const override { return fHitsInputEvent; }
    any GetOutputEvent() const override { return fHitsOutputEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " reference energy (ERef): " << fEnergyRef << RESTendl;
        RESTMetadata << " resolution at ERef : " << fResolutionAtERef << RESTendl;

        EndPrintProcess();
    }

    inline TRestMetadata* GetProcessMetadata() const { return nullptr; }

    const char* GetProcessName() const override { return "smearingProcess"; }

    inline Double_t GetEnergyReference() const { return fEnergyRef; }
    inline Double_t GetResolutionReference() const { return fResolutionAtERef; }

    TRestDetectorHitsSmearingProcess();
    TRestDetectorHitsSmearingProcess(const char* configFilename);

    ~TRestDetectorHitsSmearingProcess();

    ClassDefOverride(TRestDetectorHitsSmearingProcess, 2);
};
#endif
