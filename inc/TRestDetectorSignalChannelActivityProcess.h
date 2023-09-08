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

#ifndef RestCore_TRestDetectorSignalChannelActivityProcess
#define RestCore_TRestDetectorSignalChannelActivityProcess

#include <TH1D.h>
#include <TRestEventProcess.h>

#include "TRestDetectorReadout.h"
#include "TRestDetectorSignalEvent.h"

//! A pure analysis process to generate histograms with detector channels activity
class TRestDetectorSignalChannelActivityProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorSignalEvent input
    TRestDetectorSignalEvent* fSignalEvent;  //!

    /// A pointer to the readout metadata information accessible to TRestRun
    TRestDetectorReadout* fReadout;  //!

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    /// The value of the lower signal threshold to add it to the histogram
    Double_t fLowThreshold;

    /// The value of the higher signal threshold to add it to the histogram
    Double_t fHighThreshold;

    /// The number of bins at the daq channels histogram
    Int_t fDaqHistogramChannels;

    /// The number of bins at the readout channels histogram
    Int_t fReadoutHistogramChannels;

    /// The first channel at the daq channels histogram
    Int_t fDaqStartChannel;

    /// The last channel at the daq channels histogram
    Int_t fDaqEndChannel;

    /// The first channel at the readout channels histogram
    Int_t fReadoutStartChannel;

    /// The last channel at the readout channels histogram
    Int_t fReadoutEndChannel;

    /// The daq channels histogram
    TH1D* fDaqChannelsHisto;  //!

    /// The readout channels histogram
    TH1D* fReadoutChannelsHisto;  //!

    /// The readout channels histogram built with 1-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_OneSignal;  //!

    /// The readout channels histogram built with 1-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_OneSignal_High;  //!

    /// The readout channels histogram built with 2-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_TwoSignals;  //!

    /// The readout channels histogram built with 2-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_TwoSignals_High;  //!

    /// The readout channels histogram built with 3-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_ThreeSignals;  //!

    /// The readout channels histogram built with 3-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_ThreeSignals_High;  //!

    /// The readout channels histogram built more than 3-signal events (low threshold)
    TH1D* fReadoutChannelsHisto_MultiSignals;  //!

    /// The readout channels histogram built more than 3-signal events (high threshold)
    TH1D* fReadoutChannelsHisto_MultiSignals_High;  //!

   public:
    RESTValue GetInputEvent() const override { return fSignalEvent; }
    RESTValue GetOutputEvent() const override { return fSignalEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << "Low signal threshold activity : " << fLowThreshold << RESTendl;
        RESTMetadata << "High signal threshold activity : " << fHighThreshold << RESTendl;

        RESTMetadata << "Number of daq histogram channels : " << fDaqHistogramChannels << RESTendl;
        RESTMetadata << "Start daq channel : " << fDaqStartChannel << RESTendl;
        RESTMetadata << "End daq channel : " << fDaqEndChannel << RESTendl;

        RESTMetadata << "Number of readout histogram channels : " << fReadoutHistogramChannels << RESTendl;
        RESTMetadata << "Start readout channel : " << fReadoutStartChannel << RESTendl;
        RESTMetadata << "End readout channel : " << fReadoutEndChannel << RESTendl;

        EndPrintProcess();
    }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "SignalChannelActivity"; }

    // Constructor
    TRestDetectorSignalChannelActivityProcess();
    TRestDetectorSignalChannelActivityProcess(const char* configFilename);
    // Destructor
    ~TRestDetectorSignalChannelActivityProcess();

    ClassDefOverride(TRestDetectorSignalChannelActivityProcess, 3);
};
#endif
