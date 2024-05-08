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
/// TRestDetectorHitsSmearingProcess will introduce a stochastic smearing on the
/// energy of the hits for each event. The total energy of each event will be
/// re-distributed following a gaussian shaped function.
///
/// The energy resolution will follow a root square law with the energy
/// using the `energyReference` and the `resolutionReference` as a pivoting
/// point.
///
/// Therefore, the process defines only two parameters, one being the energy
/// at wich the energy resolution is "known", and the FWHM in percentage at
/// that given energy, `resolutionReference`.
///
/// \code
///    <addProcess type="TRestDetectorHitsSmearingProcess" name="smear"
///                title="Energy resolution smearing">
///        <parameter name="energyReference" value="250keV" />
///        <parameter name="resolutionReference" value="15" />
///    </addProcess>
/// \endcode
///
/// The following image is the result of smearing the energy of an electron
/// gun of 250keV. It was produced using the exercise 3.2 inside the
/// [day3/session2](https://github.com/rest-for-physics/rest-school/tree/master/day3/session2)
/// at the rest-school repository.
///
/// \htmlonly <style>div.image img[src="smear.png"]{width:500px;}</style> \endhtmlonly
/// ![The result of applying a 15% energy resolution at 250keV](smear.png)
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-February: First implementation of TRestDetectorHitsSmearingProcess
/// \author Javier G. Garza
///
/// 2023-January: This class has been documented
/// \author Javier Galan - javier.galan@unizar.es
///
/// \class TRestDetectorHitsSmearingProcess
///
/// <hr>
///

#include "TRestDetectorHitsSmearingProcess.h"

using namespace std;

ClassImp(TRestDetectorHitsSmearingProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestDetectorHitsSmearingProcess::TRestDetectorHitsSmearingProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestDetectorHitsSmearingProcess::~TRestDetectorHitsSmearingProcess() { delete fOutputEvent; }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the section name
///
void TRestDetectorHitsSmearingProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputEvent = nullptr;
    fOutputEvent = new TRestDetectorHitsEvent();
}

void TRestDetectorHitsSmearingProcess::InitProcess() {
    delete fRandom;
    fRandom = new TRandom3(fSeed);
    fSeed = fRandom->TRandom::GetSeed();

    fReadout = dynamic_cast<TRestDetectorReadout*>(fRunInfo->GetMetadataClass("TRestDetectorReadout"));

    if (!fReadout && !fChannelType.empty()) {
        throw runtime_error(
            "TRestDetectorHitsSmearingProcess::InitProcess() : "
            "TRestDetectorReadout not found and channel type is requested");
    }

    transform(fChannelType.begin(), fChannelType.end(), fChannelType.begin(), ::tolower);
    const auto validChannelTypes = {"veto", "tpc"};
    if (find(validChannelTypes.begin(), validChannelTypes.end(), fChannelType) == validChannelTypes.end()) {
        throw runtime_error(
            "TRestDetectorHitsSmearingProcess::InitProcess() : Channel type not valid: valid types are: "
            "'tpc', 'veto'");
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestDetectorHitsSmearingProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = (TRestDetectorHitsEvent*)inputEvent;
    fOutputEvent->SetEventInfo(fInputEvent);

    Double_t eDep = fInputEvent->GetTotalEnergy();
    Double_t eRes = fResolutionAtERef * TMath::Sqrt(fEnergyRef / eDep) / 2.35 / 100.0;

    Double_t gain = fRandom->Gaus(1.0, eRes);
    for (int hit = 0; hit < static_cast<int>(fInputEvent->GetNumberOfHits()); hit++) {
        const auto hitType = fInputEvent->GetType(hit);

        // VETO hit types are processed according the "veto" channel type. All other hit types are processed
        // according to the "tpc" type (XYZ, XZ, etc.)

        if (fChannelType == "veto") {
            if (hitType == REST_HitType::VETO) {
                fOutputEvent->AddHit(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit),
                                     fInputEvent->GetEnergy(hit) * gain, fInputEvent->GetTime(hit),
                                     fInputEvent->GetType(hit));
            } else {
                // Do nothing
                fOutputEvent->AddHit(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit),
                                     fInputEvent->GetEnergy(hit), fInputEvent->GetTime(hit),
                                     fInputEvent->GetType(hit));
            }
        } else if (fChannelType == "tpc") {
            if (hitType == REST_HitType::VETO) {
                // Do nothing
                fOutputEvent->AddHit(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit),
                                     fInputEvent->GetEnergy(hit), fInputEvent->GetTime(hit),
                                     fInputEvent->GetType(hit));
            } else {
                fOutputEvent->AddHit(fInputEvent->GetX(hit), fInputEvent->GetY(hit), fInputEvent->GetZ(hit),
                                     fInputEvent->GetEnergy(hit) * gain, fInputEvent->GetTime(hit),
                                     fInputEvent->GetType(hit));
            }
        } else {
            // this should never happen
            throw runtime_error(
                "TRestDetectorHitsSmearingProcess::ProcessEvent() : "
                "Channel type not valid: valid types are: "
                "'tpc', 'veto'");
        }
    }

    return fOutputEvent;
}
