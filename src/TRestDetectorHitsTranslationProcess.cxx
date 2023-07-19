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
/// TRestDetectorHitsTranslationProcess will move every hit inside a
/// TRestDetectorHitsEvent by an amount given at the `translation` vector.
///
/// \code
///    <addProcess type="TRestDetectorHitsTranslationProcess" name="5mmXY"
///                translation="(5,5,0)mm" />
/// \endcode
///
/// The following figure has been produced using the `translation.C`
/// defined under `detector/pipeline/hits/translation/`. On the left we
/// have the original hits event distribution. On the right the hits
/// distribution after a translation by a vector of (-1.5,0.5,0)cm.
///
/// \htmlonly <style>div.image img[src="translation.png"]{width:1000px;}</style> \endhtmlonly
/// ![The effect of translation process](translation.png)
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2023-July: First implementation of TRestDetectorHitsTranslationProcess
/// \author Javier Galan
///
/// \class TRestDetectorHitsTranslationProcess
///
/// <hr>
///
#include "TRestDetectorHitsTranslationProcess.h"

using namespace std;

#include <TRandom3.h>

ClassImp(TRestDetectorHitsTranslationProcess);

TRestDetectorHitsTranslationProcess::TRestDetectorHitsTranslationProcess() { Initialize(); }

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
TRestDetectorHitsTranslationProcess::TRestDetectorHitsTranslationProcess(const char* configFilename) {
    Initialize();
    LoadConfigFromFile(configFilename);
}

TRestDetectorHitsTranslationProcess::~TRestDetectorHitsTranslationProcess() {}

void TRestDetectorHitsTranslationProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputEvent = nullptr;
    fOutputEvent = new TRestDetectorHitsEvent();
}

TRestEvent* TRestDetectorHitsTranslationProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = (TRestDetectorHitsEvent*)inputEvent;
    fOutputEvent->SetEventInfo(fInputEvent);

    for (unsigned int hit = 0; hit < fInputEvent->GetNumberOfHits(); hit++) {
        TVector3 position(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit));

        position += fTranslation;

        fOutputEvent->AddHit(position.X(), position.Y(), position.Z(), fInputEvent->GetEnergy(hit),
                             fInputEvent->GetTime(hit), fInputEvent->GetType(hit));
    }
    return fOutputEvent;
}

void TRestDetectorHitsTranslationProcess::InitFromConfigFile() {
    TRestEventProcess::InitFromConfigFile();

    fTranslation = Get3DVectorParameterWithUnits("translation", {0, 0, 1});
}

void TRestDetectorHitsTranslationProcess::PrintMetadata() {
    BeginPrintProcess();

    RESTMetadata << " - Translation vector : ( " << fTranslation.X() << ", " << fTranslation.Y() << ", "
                 << fTranslation.Z() << ") mm" << RESTendl;

    EndPrintProcess();
}
