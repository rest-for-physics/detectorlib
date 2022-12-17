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

#ifndef RestCore_TRestDetectorSignalRecoveryProcess
#define RestCore_TRestDetectorSignalRecoveryProcess

#include <TRestDetectorReadout.h>
#include <TRestEventProcess.h>
#include "TRestDetectorSignalEvent.h"

//! A process allowing to recover selected channels from a TRestRawSignalEvent
class TRestDetectorSignalRecoveryProcess : public TRestEventProcess {
   private:
    std::vector<Int_t> fChannelIds;  //<

    /// A pointer to the specific TRestDetectorSignalEvent input
    TRestDetectorSignalEvent* fInputSignalEvent;  //!

    /// A pointer to the specific TRestDetectorSignalEvent input
    TRestDetectorSignalEvent* fOutputSignalEvent;  //!

    /// A pointer to the readout that will be extracted from TRestRun
    TRestDetectorReadout* fReadout;  //!

    void Initialize() override;

    void LoadDefaultConfig();

    void GetAdjacentSignalIds(Int_t signalId, Int_t& idLeft, Int_t& idRight);

   public:
    any GetInputEvent() const override { return fInputSignalEvent; }
    any GetOutputEvent() const override { return fOutputSignalEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* eventInput) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();
        for (const auto& channelId : fChannelIds) {
            RESTMetadata << "Channel id to recover: " << channelId << RESTendl;
        }
        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestDetectorSignalRecoveryProcess; }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "signalRecovery"; }

    // Constructor
    TRestDetectorSignalRecoveryProcess();
    TRestDetectorSignalRecoveryProcess(const char* configFilename);

    // Destructor
    ~TRestDetectorSignalRecoveryProcess();

    ClassDefOverride(TRestDetectorSignalRecoveryProcess, 1);
};
#endif
