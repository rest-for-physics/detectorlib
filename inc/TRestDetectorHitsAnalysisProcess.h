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

#ifndef RestCore_TRestDetectorHitsAnalysisProcess
#define RestCore_TRestDetectorHitsAnalysisProcess

#include <TCanvas.h>
#include <TH1D.h>
#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorReadout.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract valuable information from Hits type of data.
class TRestDetectorHitsAnalysisProcess : public TRestEventProcess {
   private:
    TRestDetectorHitsEvent* fInputHitsEvent;   //!
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    Bool_t fCylinderFiducial;  //!
    Bool_t fPrismFiducial;     //!

    void InitFromConfigFile() override;

    void Initialize() override;

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    //
    TVector3 fFid_x0;
    TVector3 fFid_x1;
    Double_t fFid_R;
    Double_t fFid_sX;
    Double_t fFid_sY;
    Double_t fFid_theta;

   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override {
        BeginPrintProcess();

        if (fCylinderFiducial) RESTMetadata << "Cylinder fiducial active" << RESTendl;
        if (fPrismFiducial) RESTMetadata << "Prism fiducial active" << RESTendl;

        RESTMetadata << " -------------------" << RESTendl;
        RESTMetadata << " Fiducial parameters" << RESTendl;
        RESTMetadata << " -------------------" << RESTendl;
        RESTMetadata << " x0 : (" << fFid_x0.X() << " , " << fFid_x0.Y() << " , " << fFid_x0.Z() << ")" << RESTendl;
        RESTMetadata << " x1 : (" << fFid_x1.X() << " , " << fFid_x1.Y() << " , " << fFid_x1.Z() << ")" << RESTendl;
        RESTMetadata << " R : " << fFid_R << RESTendl;
        RESTMetadata << " sX : " << fFid_sX << RESTendl;
        RESTMetadata << " sY : " << fFid_sY << RESTendl;
        RESTMetadata << " -------------------" << RESTendl;

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "hitsAnalysis"; }

    TRestDetectorHitsAnalysisProcess();
    TRestDetectorHitsAnalysisProcess(const char* configFilename);

    ~TRestDetectorHitsAnalysisProcess();

    ClassDefOverride(TRestDetectorHitsAnalysisProcess, 1);
};
#endif
