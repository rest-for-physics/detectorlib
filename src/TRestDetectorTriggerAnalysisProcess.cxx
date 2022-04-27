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

//////////////////////////////////////////////////////////////////////////
///
/// This process will receive as input a physical time signal, then it will
/// use a fixed time window defined by the sampling time in us, and the the
/// sampling depth. It will scan the physical signal using the virtual ADC
/// defined window, and if the total energy integral inside the window is
/// above the threshold it will define a non-zero value for the corresponding
/// integral observable.
///
/// This process was one day used to optimize the efficiency of NLDBD events
/// when registering events in a fixed-size time window, at a given energy
/// window threshold. It must be noted that it may happen that the event extension
/// is larger than the chosen window size.
///
/// Therefore, when when we define an observable for a given energy threshold, i.e.
/// Integral_Eth2, for 2keV energy threshold, we are in fact determining the first
/// moving ADC window where the accumulative integral on that window is above 2keV.
///
/// The resulting observable integral will be more deeply connected with the
/// electronics response, since the physical event could be extended in a wider
/// physical time window.
///
/// We need to define manually the values for the threshold inside the integral
/// observable:
///
/// \code
///    <TRestDetectorTriggerAnalysisProcess name="trigger">
///		   // It will define a trigger window when the accumulative integral
///        // gets above 10keV.
///		   <observable name="Integral_Eth10" type="double" value="ON" />
///    </TRestDetectorTriggerAnalysisProcess>
/// \endcode
///
/// This process is very similar to TRestDetectorSignalToRawSignalProcess. Altrhought,
/// this process is just a pure analysis process producing few observables without touching
/// the event, while the later is used as a transformation process used to produce a raw
/// signal event type with the possibility of using a threshold definition.
///
/// <hr>
///
/// \warning **⚠ REST is under continous development.** This documentation
/// is offered to you by the REST community. Your HELP is needed to keep this code
/// up to date. Your feedback will be worth to support this software, please report
/// any problems/suggestions you may find while using it at [The REST Framework
/// forum](http://ezpc10.unizar.es). You are welcome to contribute fixing typos,
/// updating information or adding/proposing new contributions. See also our
/// <a href="https://github.com/rest-for-physics/framework/blob/master/CONTRIBUTING.md">Contribution
/// Guide</a>.
///
///
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-April: First implementation of trigger analysis process
///             Javier Galan
///
/// \class      TRestDetectorTriggerAnalysisProcess
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestDetectorTriggerAnalysisProcess.h"

using namespace std;

ClassImp(TRestDetectorTriggerAnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorTriggerAnalysisProcess::TRestDetectorTriggerAnalysisProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param configFilename A const char* giving the path to an RML file.
///
TRestDetectorTriggerAnalysisProcess::TRestDetectorTriggerAnalysisProcess(const char* configFilename) {
    Initialize();
    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorTriggerAnalysisProcess::~TRestDetectorTriggerAnalysisProcess() { delete fSignalEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorTriggerAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestDetectorTriggerAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fSignalEvent = new TRestDetectorSignalEvent();
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param configFilename A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// correspondig TRestGeant4AnalysisProcess section inside the RML.
///
void TRestDetectorTriggerAnalysisProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization. Observable names are interpreted here
///
void TRestDetectorTriggerAnalysisProcess::InitProcess() {
    std::vector<string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();

    for (unsigned int i = 0; i < fObservables.size(); i++)
        if (fObservables[i].find("Integral_Eth") != string::npos)
            fIntegralObservables.push_back(fObservables[i]);

    for (unsigned int i = 0; i < fIntegralObservables.size(); i++) {
        Double_t th = atof(fIntegralObservables[i].substr(12, fIntegralObservables[i].length() - 12).c_str());
        fThreshold.push_back(th);
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorTriggerAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    TRestDetectorSignalEvent* fInputSignalEvent = (TRestDetectorSignalEvent*)evInput;
    fSignalEvent->SetEventInfo(fInputSignalEvent);
    for (int sgnl = 0; sgnl < fInputSignalEvent->GetNumberOfSignals(); sgnl++)
        fSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(sgnl));

    vector<Double_t> integral;
    for (unsigned int i = 0; i < fIntegralObservables.size(); i++) integral.push_back(0);

    Int_t maxT = fSignalEvent->GetMaxTime() / fSampling;  // convert to time bin
    Int_t minT = fSignalEvent->GetMinTime() / fSampling;

    Int_t triggerStarts = 0;

    unsigned int counter = 0;
    unsigned int nObs = fIntegralObservables.size();
    for (int i = minT - fADCLength / 2; i <= maxT && counter < nObs; i++) {
        Double_t en = fSignalEvent->GetIntegralWithTime(i * fSampling, (i + fADCLength / 2) * fSampling);

        for (unsigned int n = 0; n < nObs; n++)
            if (integral[n] == 0 && en > fThreshold[n]) {
                // We define the trigger start only for the first threshold definition
                if (n == 0) triggerStarts = i;
                integral[n] = fSignalEvent->GetIntegralWithTime(i * fSampling, (i + fADCLength) * fSampling);
            }

        // Break condition
        counter = 0;
        for (unsigned int n = 0; n < nObs; n++)
            if (integral[n] > 0) counter++;
    }

    for (unsigned int i = 0; i < nObs; i++) {
        SetObservableValue(fIntegralObservables[i], integral[i]);
    }

    Double_t full = fSignalEvent->GetIntegralWithTime(minT * fSampling - 1, maxT * fSampling + 1);

    SetObservableValue("RawIntegral", full);
    SetObservableValue("TriggerStarts", triggerStarts);

    if (fSignalEvent->GetNumberOfSignals() <= 0) return fSignalEvent;

    return fSignalEvent;
}
