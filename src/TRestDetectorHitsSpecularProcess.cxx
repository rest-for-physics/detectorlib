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
/// TRestDetectorHitsSpecularProcess will update the hits position using
/// a specular image respect to a plane defined using its `normal` and
/// a `position` that is contained in the plane.
///
/// \code
///    <addProcess type="TRestDetectorHitsSpecularProcess" name="spec45"
///                title="A 45 degrees specular hits image">
///        <parameter name="position" value="(0,0,0)" />
///        <parameter name="normal" value="(0.5,0.5,0)" />
///    </addProcess>
/// \endcode
///
/// The following figure has been produced using the `specular.C` defined
/// under `detector/pipeline/hits/specular/`. On the left-top corner we
/// have the original hits event distribution. On the right-top the hits
/// using a specular plane vector defined as (1,1,0) (or y=-x). On the
/// left-bottom a specular image respect to the X-axis, using the normal
/// plane vector (0,1,0). On the right-bottom image the same axis is used
/// but the plane position has been shifted by 5mm to the bottom. Black
/// lines identify the specular planes.
///
/// \htmlonly <style>div.image img[src="specular.png"]{width:800px;}</style> \endhtmlonly
/// ![The effect of different specular processes](specular.png)
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2023-July: First implementation of TRestDetectorHitsSpecularProcess
/// \author Javier Galan
///
/// \class TRestDetectorHitsSpecularProcess
///
/// <hr>
///
#include "TRestDetectorHitsSpecularProcess.h"

using namespace std;

#include <TRandom3.h>

ClassImp(TRestDetectorHitsSpecularProcess);

TRestDetectorHitsSpecularProcess::TRestDetectorHitsSpecularProcess() { Initialize(); }

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
TRestDetectorHitsSpecularProcess::TRestDetectorHitsSpecularProcess(const char* configFilename) {
    Initialize();
    LoadConfigFromFile(configFilename);
}

TRestDetectorHitsSpecularProcess::~TRestDetectorHitsSpecularProcess() {}

void TRestDetectorHitsSpecularProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputEvent = nullptr;
    fOutputEvent = new TRestDetectorHitsEvent();
}

TRestEvent* TRestDetectorHitsSpecularProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = (TRestDetectorHitsEvent*)inputEvent;
    fOutputEvent->SetEventInfo(fInputEvent);

    for (unsigned int hit = 0; hit < fInputEvent->GetNumberOfHits(); hit++) {
        TVector3 position(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit));

        TVector3 V = position - fPosition;
        Double_t vByN = V.Dot(fNormal);
        TVector3 reflectionVector = V - 2 * vByN * fNormal;

        position = fPosition + reflectionVector;

        fOutputEvent->AddHit(position.X(), position.Y(), position.Z(), fInputEvent->GetEnergy(hit),
                             fInputEvent->GetTime(hit), fInputEvent->GetType(hit));
    }
    return fOutputEvent;
}

void TRestDetectorHitsSpecularProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();

    fNormal = Get3DVectorParameterWithUnits("normal", {0, 0, 1});
    fNormal = fNormal.Unit();
    fPosition = Get3DVectorParameterWithUnits("position", {0, 0, 1});
}

void TRestDetectorHitsSpecularProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << " - Plane normal : ( " << fNormal.X() << ", " << fNormal.Y() << ", " << fNormal.Z()
                 << ") mm" << RESTendl;
    RESTMetadata << " - Plane position : ( " << fPosition.X() << ", " << fPosition.Y() << ", "
                 << fPosition.Z() << ")" << RESTendl;

    EndPrintProcess();
}
