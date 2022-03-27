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
/// This class is intended to serve as storage of basic detector properties. Basic members
/// exist with common information to most of the detector setups. More specialized detector
/// setups might inherit from this class to extend the detector description.
///
/// <hr>
///
/// \warning **⚠ REST is under continous development.** This
/// documentation /// is offered to you by the REST community. Your HELP is needed to keep this
/// code up to date. Your feedback will be worth to support this software, please
/// report any problems/suggestions you may find will using it at [The REST Framework
/// forum](http://ezpc10.unizar.es). You are welcome to contribute fixing typos,
/// updating information or adding/proposing new contributions. See also our
/// <a href="https://github.com/rest-for-physics/framework/blob/master/CONTRIBUTING.md">Contribution
/// Guide</a>.
///
///
///_______________________________________________________________________________
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-April. First implementation
///
/// \class      TRestDetector
/// \author     Javier Galan
/// \author     Kaixiang Ni
/// \author     David Diez
///
/// <hr>
///
#include "TRestDetector.h"

void TRestDetector::PrintMetadata() {
    TRestMetadata::PrintMetadata();
    any cl = any(this, this->ClassName());
    for (int i = 0; i < cl.GetNumberOfDataMembers(); i++) {
        if (cl.GetDataMember(i).name == "fgIsA") continue;
        if (cl.GetDataMember(i).ToString() != "-1")
            metadata << cl.GetDataMember(i).name << ": " << cl.GetDataMember(i).ToString() << endl;
    }
    metadata << "---------------------------------------" << endl;
}

void TRestDetector::UpdateMetadataMembers() {
    // We divide 100Mhz by the clock divider for AGET electronics.
    // We could have in future an additional member to say the electronics we are using
    if (fElectronicsClock != "-1") {
        fElectronicsSamplingTime = StringToInteger(fElectronicsClock) / 100.;
    }
}

using namespace std;

ClassImp(TRestDetector);
