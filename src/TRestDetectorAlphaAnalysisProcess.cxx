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
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-June: First implementation of hits analysis process into REST_v2.
///
/// \class      TRestDetectorAlphaAnalysisProcess
/// \author     David Diez
///
/// <hr>
///
//////////////////////////////////////////////////////////////////////////

#include "TRestDetectorAlphaAnalysisProcess.h"
using namespace std;

ClassImp(TRestDetectorAlphaAnalysisProcess);
///////////////////////////////////////////////
/// \brief  Default constructor
///
TRestDetectorAlphaAnalysisProcess::TRestDetectorAlphaAnalysisProcess() { Initialize(); }

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
/// \param cfgFileName A const char* giving the path to an RML file. 
///
TRestDetectorAlphaAnalysisProcess::TRestDetectorAlphaAnalysisProcess(char* cfgFileName) {
    Initialize();

    if (LoadConfigFromFile(cfgFileName) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorAlphaAnalysisProcess::~TRestDetectorAlphaAnalysisProcess() {}

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestDetectorAlphaAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestDetectorAlphaAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fHitsEvent = NULL;

}

void TRestDetectorAlphaAnalysisProcess::LoadConfig(std::string cfgFilename, std::string name) {
    if (LoadConfigFromFile(cfgFilename, name) == -1) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestDetectorAlphaAnalysisProcess::InitProcess() { TRestEventProcess::ReadObservables(); }

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorAlphaAnalysisProcess::ProcessEvent(TRestEvent* evInput) {
    fHitsEvent = (TRestDetectorHitsEvent*)evInput;
    
    TRestHits* Xhits = fHitsEvent->GetXZHits();
    TRestHits* Yhits = fHitsEvent->GetYZHits();
    TRestHits* hits = fHitsEvent->GetHits();
    
    // Direction of the track. One hit per signal (onlyMax or qCenter. For tripleMax could work as well, probably not for "none")
    double Tmax = 0, Tmin = 500, EMax = 0, EMaxPos = 0, D = -1;
    
    // GetTime() gives 0 in our recient procesed files, that's why GetZ() instead. Be careful with the sign.
    for (int j=0; j<hits->GetNumberOfHits(); j++){
        if (j==0){Tmax=-hits->GetZ(j); Tmin=-hits->GetZ(j); }
        if(-hits->GetZ(j) > Tmax){Tmax = -hits->GetZ(j);}
        if(-hits->GetZ(j) < Tmin){Tmin = -hits->GetZ(j);}
        if (hits->GetEnergy(j) > EMax) {EMaxPos = -hits->GetZ(j); EMax = hits->GetEnergy(j);}
    }
    
    D = (double)(EMaxPos-Tmin)/(Tmax - Tmin);
    SetObservableValue("directionUpDown", D);
    
    
    // Initial and end points, length
    double minZx = 500, maxZx = -500, minZy = 500, maxZy = -500, xUp = 0, yUp = 0, xDw = 0, yDw = 0, xDwEnd = 0, yDwEnd = 0, xUpEnd = 0, yUpEnd = 0, L = -1;
    // Down
    if(D<0.5){
        for (int j=0; j<Xhits->GetNumberOfHits(); j++){
            if(-Xhits->GetZ(j) > maxZx){xDw = Xhits->GetX(j); maxZx = -Xhits->GetZ(j);}
            if(-Xhits->GetZ(j) < minZx){xDwEnd = Xhits->GetX(j); minZx = -Xhits->GetZ(j);}
        }
        for (int j=0; j<Yhits->GetNumberOfHits(); j++){
            if(-Yhits->GetZ(j) > maxZy){yDw = Yhits->GetY(j); maxZy = -Yhits->GetZ(j);}
            if(-Yhits->GetZ(j) < minZy){yDwEnd = Yhits->GetY(j); minZy = -Yhits->GetZ(j);} 
        }
        SetObservableValue("beginTrackX", xDw);
        SetObservableValue("beginTrackY", yDw);
        SetObservableValue("endTrackX", xDwEnd);
        SetObservableValue("endTrackY", yDwEnd);
        
        L = sqrt(pow(xDw-xDwEnd, 2)+pow(yDw-yDwEnd, 2)+pow(max(maxZx,maxZy)-min(minZx,minZy), 2));
        SetObservableValue("lengthTrack", L);
        
    }
    // Up
    if(D>=0.5){
        for (int j=0; j<Xhits->GetNumberOfHits(); j++){
            if(-Xhits->GetZ(j) < minZx){xUp = Xhits->GetX(j); minZx = -Xhits->GetZ(j);} 
            if(-Xhits->GetZ(j) > maxZx){xUpEnd = Xhits->GetX(j); maxZx = -Xhits->GetZ(j);}
        }
        for (int j=0; j<Yhits->GetNumberOfHits(); j++){
            if(-Yhits->GetZ(j) < minZy){yUp = Yhits->GetY(j); minZy = -Yhits->GetZ(j);}
            if(-Yhits->GetZ(j) > maxZy){yUpEnd = Yhits->GetY(j); maxZy = -Yhits->GetZ(j);} 
        }
        
        SetObservableValue("beginTrackX", xUp);
        SetObservableValue("beginTrackY", yUp);
        SetObservableValue("endTrackX", xUpEnd);
        SetObservableValue("endTrackY", yUpEnd);
        
        L = sqrt(pow(xUp-xUpEnd, 2)+pow(yUp-yUpEnd, 2)+pow(max(maxZx,maxZy)-min(minZx,minZy), 2));
        SetObservableValue("lengthTrack", L);
    }
    
     // Energy per unit lenght
    

    return fHitsEvent;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestDetectorAlphaAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief  Function reading input parameters from the RML
/// TRestDetectorAlphaAnalysisProcess section
///
void TRestDetectorAlphaAnalysisProcess::InitFromConfigFile() {
    /*fFid_x0 = Get3DVectorParameterWithUnits("fiducial_x0", TVector3(0, 0, 0));
    fFid_x1 = Get3DVectorParameterWithUnits("fiducial_x1", TVector3(0, 0, 0));

    fFid_R = GetDblParameterWithUnits("fiducial_R", 1);
    fFid_sX = GetDblParameterWithUnits("fiducial_sX", 1);
    fFid_sY = GetDblParameterWithUnits("fiducial_sY", 1);
    // read angle in degrees
    fFid_theta = StringToDouble(GetParameter("fiducial_theta", "0"));
    // convert it into radians for internal usage
    fFid_theta = std::fmod(fFid_theta, 360) * TMath::DegToRad();

    if (GetParameter("cylinderFiducialization", "false") == "true") fCylinderFiducial = true;

    if (GetParameter("prismFiducialization", "false") == "true") fPrismFiducial = true;*/
}
