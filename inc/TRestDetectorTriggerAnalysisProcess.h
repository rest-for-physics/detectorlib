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

#ifndef RestCore_TRestDetectorTriggerAnalysisProcess
#define RestCore_TRestDetectorTriggerAnalysisProcess

#include <TRestEventProcess.h>

#include "TRestDetectorSignalEvent.h"

//! A process to generate integral observables for signal ADC windows found above the defined energy threshold
class TRestDetectorTriggerAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestSignalEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A std::vector to temporary store the name of threshold observables
    std::vector<std::string> fIntegralObservables;  //!

    /// A std::vector to temporary the extracted threshold value from the corresponding observable
    std::vector<double> fThreshold;  //!

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    /// It defines the sampling time in us to treat the signal as if it would be a rawsignal.
    Double_t fSampling = 0.2;

    /// It defines the number of bins used to define the sampling depth.
    Int_t fADCLength = 512;

   public:
    /// Returns a pointer to the input signal event
    RESTValue GetInputEvent() const override { return fSignalEvent; }

    /// Returns a pointer to the input signal event
    RESTValue GetOutputEvent() const override { return fSignalEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// Prints on screen the metadata information registered by this process
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Sampling : " << fSampling << " us" << RESTendl;
        RESTMetadata << "ADC length : " << fADCLength << RESTendl;

        EndPrintProcess();
    }

    /// Returns a std::string with the process name
    const char* GetProcessName() const override { return "triggerAnalysis"; }

    TRestDetectorTriggerAnalysisProcess();
    TRestDetectorTriggerAnalysisProcess(const char* configFilename);

    ~TRestDetectorTriggerAnalysisProcess();

    ClassDefOverride(TRestDetectorTriggerAnalysisProcess, 2);
};
#endif
