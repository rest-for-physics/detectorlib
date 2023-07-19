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
    fNormal.Unit();
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
