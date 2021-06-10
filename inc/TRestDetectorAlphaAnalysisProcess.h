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

#ifndef RestCore_TRestDetectorAlphaAnalysisProcess
#define RestCore_TRestDetectorAlphaAnalysisProcess

//#include <TH1D.h>
#include <math.h>

//#include <TCanvas.h>

//#include <TRestDetectorReadout.h>
//#include <TRestDetectorGas.h>
#include <TRestDetectorHitsEvent.h>
#include <TRestDetectorSignalEvent.h>

#include "TRestEventProcess.h"

//! An analysis REST process to extract information from alpha events in Hits type of data.
class TRestDetectorAlphaAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestDetectorHitsEvent* fHitsEvent;   //!

#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process
    //

   public:
    any GetInputEvent() { return fHitsEvent; }
    any GetOutputEvent() { return fHitsEvent; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(std::string cfgFilename, std::string name = "");

    void PrintMetadata() {
        BeginPrintProcess();


        EndPrintProcess();
    }

    TString GetProcessName() { return (TString) "alphaAnalysis"; }

    // Constructor
    TRestDetectorAlphaAnalysisProcess();
    TRestDetectorAlphaAnalysisProcess(char* cfgFileName);
    // Destructor
    ~TRestDetectorAlphaAnalysisProcess();

    ClassDef(TRestDetectorAlphaAnalysisProcess, 1);  // Template for a REST "event process" class inherited from
                                            // TRestEventProcess
};
#endif
