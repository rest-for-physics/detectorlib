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
/// The TRestDetectorSignalRecoveryProcess allows to recover a selection
/// of daq channel ids from a TRestDetectorSignalEvent. The dead channels must
/// be known beforehand and the signal ids to be recovered must be
/// specified through the corresponding section at the RML configuration
/// file.
///
/// The following example will apply the recovery algorithm for the
/// channels with signal ids 17,19,27 and 67. The signal ids must exist
/// in the readout defined through the TRestDetectorReadout structure.
///
/// \code
/// <TRestDetectorSignalRecoveryProcess name="returnChannels"
/// title="Recovering few channels" verboseLevel="debug" >
///     <parameter name="channelIds" value="{17,27,67}" />
/// </TRestDetectorSignalRecoveryProcess>
/// \endcode
///
/// The dead channel reconstruction algorithm is for the moment very
/// simple. The charge of the dead channel is directly calculated by
/// using the charge of the adjacent readout channels,
/// \f$s_i = 0.5 \times (s_{i-1} + s_{i+1})\f$
///
/// Now we preserve the energy of the event, we apply the following
/// correction to the dead channel (i) and the adjacent channels.
///
/// \f$s_{i-1} = 0.5 * s_{i-1}\f$
/// \f$s_i = 0.5 \times (s_{i-1} + s_{i+1})\f$
/// \f$s_{i+1} = 0.5 * s_{i+1}\f$
//
/// The algorithm will also identify in case we have up to two
/// consecutive dead channels. In that case it will apply the following
/// correction, where `i` and `i+1` are the dead channels.
///
/// \f$s_{i-1} = 0.5 * s_{i-1}\f$
/// \f$s_i = 1/6 \times (2*s_{i-1} + s_{i+2})\f$
/// \f$s_{i+1} = 1/6 \times (s_{i-1} + 2*s_{i+2})\f$
/// \f$s_{i+2} = 0.5 * s_{i+2}\f$
///
/// This process will access the information of the decoding stored in
/// the TRestDetectorReadout definition to assure that the righ signal ids,
/// corresponding to the adjacent channels, are used in the calculation.
///
/// \warning This process will only be functional if the detectorlib
/// was compiled. You may check if it is the case executing the command
/// `rest-config --libs`, and checking the output shows `-lRestDetector`.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-November: First implementation of TRestDetectorSignalRecoveryProcess.
///                 Originally it was TRestRawSignalRecoveryProcess.
///             Javier Galan
///
/// 2023-February: Update of the process so that it preserves energy and takes into
///                account two consecutive dead channels.
///         	Ana Quintana/Javier Galan
///
/// \class      TRestDetectorSignalRecoveryProcess
/// \author     Javier Galan
/// \author     Ana Quintana
///
/// <hr>
///
#include "TRestDetectorSignalRecoveryProcess.h"

using namespace std;

ClassImp(TRestDetectorSignalRecoveryProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorSignalRecoveryProcess::TRestDetectorSignalRecoveryProcess() { Initialize(); }

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
TRestDetectorSignalRecoveryProcess::TRestDetectorSignalRecoveryProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename) == -1) LoadDefaultConfig();

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorSignalRecoveryProcess::~TRestDetectorSignalRecoveryProcess() { delete fOutputSignalEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorSignalRecoveryProcess::LoadDefaultConfig() {
    SetName("removeChannels-Default");
    SetTitle("Default config");
}

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestDetectorSignalRecoveryProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputSignalEvent = nullptr;
    fOutputSignalEvent = new TRestDetectorSignalEvent();
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
void TRestDetectorSignalRecoveryProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Function to initialize the process.
/// TRestDetectorSignalRecoveryProcess requires to get a pointer to
/// TRestDetectorReadout.
///
void TRestDetectorSignalRecoveryProcess::InitProcess() {
    fReadout = GetMetadata<TRestDetectorReadout>();

    if (fReadout == nullptr) {
        RESTError << "TRestDetectorSignalRecoveryProcess. Readout has not been initialized!" << RESTendl;
        exit(-1);
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorSignalRecoveryProcess::ProcessEvent(TRestEvent* evInput) {
    fInputSignalEvent = (TRestDetectorSignalEvent*)evInput;

    for (int n = 0; n < fInputSignalEvent->GetNumberOfSignals(); n++)
        fOutputSignalEvent->AddSignal(*fInputSignalEvent->GetSignal(n));

    Int_t nPoints = fOutputSignalEvent->GetSignal(0)->GetNumberOfPoints();

    Int_t idL;
    Int_t idR;
    for (unsigned int x = 0; x < fChannelIds.size(); x++) {
        Int_t type = GetAdjacentSignalIds(fChannelIds[x], idL, idR);
        RESTDebug << "Channel id : " << fChannelIds[x] << " Left : " << idL << " Right : " << idR << RESTendl;

        if (fOutputSignalEvent->GetSignalIndex(fChannelIds[x]) > 0)
            fOutputSignalEvent->RemoveSignalWithId(fChannelIds[x]);

        if (idL == -1 || idR == -1) continue;

        TRestDetectorSignal* leftSgnl = fInputSignalEvent->GetSignalById(idL);
        TRestDetectorSignal* rightSgnl = fInputSignalEvent->GetSignalById(idR);

        /// If the dead channel has no charge on left and right then we do not
        /// correct. We could think about correction here? It means it is
        /// the first or last channel.
        ///
        if (leftSgnl == nullptr || rightSgnl == nullptr) continue;

        TRestDetectorSignal* recoveredSignal = new TRestDetectorSignal();
        recoveredSignal->SetID(fChannelIds[x]);

        if (type == 1)  // Only one dead channel
        {
            for (int n = 0; n < leftSgnl->GetNumberOfPoints(); n++) {
                recoveredSignal->IncreaseAmplitude(leftSgnl->GetPoint(n) / 2.);
                /// Energy preserved. This could be optional using a new metadata member
                leftSgnl->IncreaseAmplitude(-1. * leftSgnl->GetPoint(n) / 2);
            }

            for (int n = 0; n < rightSgnl->GetNumberOfPoints(); n++) {
                recoveredSignal->IncreaseAmplitude(rightSgnl->GetPoint(n) / 2.);
                /// Energy preserved. This could be optional using a new metadata member
                rightSgnl->IncreaseAmplitude(-1. * rightSgnl->GetPoint(n) / 2);
            }
        }

        if (type == 2 || type == 3)  // We got two dead-channels
        {
            if (type == 2)  // The dead channel is the one at the right
            {
                for (int n = 0; n < leftSgnl->GetNumberOfPoints(); n++)
                    recoveredSignal->IncreaseAmplitude(leftSgnl->GetPoint(n) / 6.);

                for (int n = 0; n < rightSgnl->GetNumberOfPoints(); n++)
                    recoveredSignal->IncreaseAmplitude(2 * rightSgnl->GetPoint(n) / 6.);
            }

            if (type == 3)  // The dead channel is the one at the left
            {
                for (int n = 0; n < leftSgnl->GetNumberOfPoints(); n++)
                    recoveredSignal->IncreaseAmplitude(2 * leftSgnl->GetPoint(n) / 6.);

                for (int n = 0; n < rightSgnl->GetNumberOfPoints(); n++)
                    recoveredSignal->IncreaseAmplitude(rightSgnl->GetPoint(n) / 6.);
            }

            /// We removed the charge that we place at the dead channel
            /// In this case we remove a 25% because we will enter twice in this loop
            for (int n = 0; n < leftSgnl->GetNumberOfPoints(); n++)
                leftSgnl->IncreaseAmplitude(-1. * leftSgnl->GetPoint(n) / 4);
            for (int n = 0; n < rightSgnl->GetNumberOfPoints(); n++)
                rightSgnl->IncreaseAmplitude(-1. * rightSgnl->GetPoint(n) / 4);
        }

        fOutputSignalEvent->AddSignal(*recoveredSignal);

        delete recoveredSignal;

        RESTDebug << "Channel recovered!! " << RESTendl;
        if (leftSgnl != nullptr && rightSgnl != nullptr)
            for (int n = 0; n < nPoints; n++)
                RESTDebug << "Sample " << n << " : " << leftSgnl->GetData(n) << " + " << rightSgnl->GetData(n)
                          << " = " << recoveredSignal->GetData(n) << RESTendl;
    }

    RESTDebug << "Channels after : " << fOutputSignalEvent->GetNumberOfSignals() << RESTendl;

    return fOutputSignalEvent;
}

///////////////////////////////////////////////
/// \brief It returns the channel daq id of the adjacent readout channels. It will properly
/// identify that we got two dead channels, but no more than two.
///
/// It returns a value that determines the case we are considering.
///
/// 0: Something went wrong (channels not found?)
/// 1: There is only one dead channel
/// 2: There is an additional dead channel to the left
/// 3: There is an additional dead channel to the right
///
int TRestDetectorSignalRecoveryProcess::GetAdjacentSignalIds(Int_t signalId, Int_t& idLeft, Int_t& idRight) {
    idLeft = -1;
    idRight = -1;

    for (int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++) {
        TRestDetectorReadoutPlane* plane = fReadout->GetReadoutPlane(p);
        for (int m = 0; m < plane->GetNumberOfModules(); m++) {
            TRestDetectorReadoutModule* mod = plane->GetModule(m);
            // We iterate over all readout modules searching for the one that contains
            // our signal id
            if (mod->isDaqIDInside(signalId)) {
                // If we find it we use the readoutModule id, and the signalId
                // corresponding to the physical readout channel
                Int_t readoutChannelID = mod->DaqToReadoutChannel(signalId);

                idLeft = mod->GetChannel(readoutChannelID - 1)->GetDaqID();
                idRight = mod->GetChannel(readoutChannelID + 1)->GetDaqID();

                // If idLeft is a dead channel we take the previous channel
                if (std::find(fChannelIds.begin(), fChannelIds.end(), idLeft) != fChannelIds.end()) {
                    idLeft = mod->GetChannel(readoutChannelID - 2)->GetDaqID();
                    return 2;
                }

                // If idRight is a dead channel we take the next channel
                if (std::find(fChannelIds.begin(), fChannelIds.end(), idLeft) != fChannelIds.end()) {
                    idRight = mod->GetChannel(readoutChannelID + 2)->GetDaqID();
                    return 3;
                }

                return 1;
            }
        }
    }
    return 0;
}
