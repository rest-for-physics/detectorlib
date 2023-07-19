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
/// TRestDetectorHitsRotationProcess will add a rotation of the x,y,z hits
/// in the space. The rotation will be done respect to a center and an
/// axis provided by the user.
///
/// \code
///    <addProcess type="TRestDetectorHitsRotationProcess" name="rotZ45"
///                title="A 45 degrees rotation">
///        <parameter name="axis" value="(0,0,1)" />
///        <parameter name="center" value="(3,3,0)" />
///        <parameter name="angle" value="45degrees" />
///    </addProcess>
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2023-July: First implementation of TRestDetectorHitsRotationProcess
/// \author Javier Galan
///
/// \class TRestDetectorHitsRotationProcess
///
/// <hr>
///
#include "TRestDetectorHitsRotationProcess.h"

using namespace std;

#include <TRandom3.h>

ClassImp(TRestDetectorHitsRotationProcess);

TRestDetectorHitsRotationProcess::TRestDetectorHitsRotationProcess() { Initialize(); }

TRestDetectorHitsRotationProcess::~TRestDetectorHitsRotationProcess() {}

void TRestDetectorHitsRotationProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputEvent = nullptr;
    fOutputEvent = new TRestDetectorHitsEvent();
}

TRestEvent* TRestDetectorHitsRotationProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = (TRestDetectorHitsEvent*)inputEvent;
    fOutputEvent->SetEventInfo(fInputEvent);

    for (unsigned int hit = 0; hit < fInputEvent->GetNumberOfHits(); hit++) {
        TVector3 position(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit));

        position -= fCenter;
        position.Rotate(position, fAxis);
        position += fCenter;

        fOutputEvent->AddHit(position.X(), position.Y(), position.Z(), fInputEvent->GetEnergy(hit),
                             fInputEvent->GetTime(hit), fInputEvent->GetType(hit));
    }
    return fOutputEvent;
}

void TRestDetectorHitsRotationProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();

    fAxis = Get3DVectorParameterWithUnits("axis", {0, 0, 1});
    fCenter = Get3DVectorParameterWithUnits("center", {0, 0, 1});
}

void TRestDetectorHitsRotationProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << " - Rotation center : ( " << fCenter.X() << ", " << fCenter.Y() << ", " << fCenter.Z()
                 << ") mm" << RESTendl;
    RESTMetadata << " - Rotation axis : ( " << fAxis.X() << ", " << fAxis.Y() << ", " << fAxis.Z() << ")"
                 << RESTendl;
    RESTMetadata << " - Rotation angle : " << fAngle * units("degrees") << " degrees" << RESTendl;

    EndPrintProcess();
}
