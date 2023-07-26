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
///        <parameter name="center" value="(0,0,0)" />
///        <parameter name="angle" value="45degrees" />
///    </addProcess>
/// \endcode
///
/// \warning This process will only be valid for those detector hits
/// of type XYZ, where the (x,y,z) coordinates take simultaneously physical
/// values. When the hits contain a non-valid (undetermined) coordinate
/// component this transformation will have non-sense and this process
/// will do nothing.
///
/// The following figure has been produced using the `rotation.C` defined
/// under `detector/pipeline/hits/rotation/`. On the left we have the
/// original hits event distribution. On the middle the hits
/// with a 90 degrees clock-wise rotation with center at (0,0,0). On the
/// right the original hits after a clock-wise rotation of 45 degrees with
/// center at (-25,25,0). The axis for both rotations is along the Z-axis.
///
/// \htmlonly <style>div.image img[src="rotation.png"]{width:1000px;}</style> \endhtmlonly
/// ![The effect of different rotation processes](rotation.png)
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
TRestDetectorHitsRotationProcess::TRestDetectorHitsRotationProcess(const char* configFilename) {
    Initialize();
    LoadConfigFromFile(configFilename);
}

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

    Bool_t xyzEvent = fInputEvent->GetXYZHits()->GetNumberOfHits() == 0 ? false : true;
    for (unsigned int hit = 0; hit < fInputEvent->GetNumberOfHits(); hit++) {
        TVector3 position(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit));

        if (xyzEvent) {
            position -= fCenter;
            position.Rotate(fAngle, fAxis);
            position += fCenter;
        } else {
            this->SetError("TRestDetectorHitsRotationProcess. Only XYZ hits can be transformed");
        }

        fOutputEvent->AddHit(position.X(), position.Y(), position.Z(), fInputEvent->GetEnergy(hit),
                             fInputEvent->GetTime(hit), fInputEvent->GetType(hit));
    }
    return fOutputEvent;
}

void TRestDetectorHitsRotationProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();

    fAxis = Get3DVectorParameterWithUnits("axis", {0, 0, 1});
    fAxis.Unit();
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
