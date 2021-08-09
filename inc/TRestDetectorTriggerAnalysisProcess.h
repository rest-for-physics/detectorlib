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

#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! A process to generate integral observables for signal ADC windows found above the defined energy threshold
class TRestDetectorTriggerAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestSignalEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A vector to temporary store the name of threshold observables
    std::vector<std::string> fIntegralObservables;  //!

    /// A vector to temporary the extracted threshold value from the corresponding observable
    std::vector<double> fThreshold;  //!

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    /// It defines the sampling time in us to treat the signal as if it would be a rawsignal.
    Double_t fSampling;

    /// It defines the number of bins used to define the sampling depth.
    Int_t fADCLength;

   public:
    /// Returns a pointer to the input signal event
    any GetInputEvent() { return fSignalEvent; }

    /// Returns a pointer to the input signal event
    any GetOutputEvent() { return fSignalEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);

    void LoadConfig(std::string cfgFilename, std::string name = "");

    /// Prints on screen the metadata information registered by this process
    void PrintMetadata() {
        BeginPrintProcess();

        metadata << "Sampling : " << fSampling << " us" << endl;
        metadata << "ADC length : " << fADCLength << endl;

        EndPrintProcess();
    }

    /// Returns a string with the process name
    TString GetProcessName() { return (TString) "triggerAnalysis"; }

    TRestDetectorTriggerAnalysisProcess();
    TRestDetectorTriggerAnalysisProcess(char* cfgFileName);

    ~TRestDetectorTriggerAnalysisProcess();

    ClassDef(TRestDetectorTriggerAnalysisProcess, 2);
};
#endif
