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
/// This class stores the readout pixel geometrical description, origin
/// position, orientation, and size. A readout pixel is the most elementary
/// component of a readout used to construct a readout channel.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-aug:  First concept.
///            Javier Galan
///
/// \class      TRestDetectorReadoutChannel
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestDetectorReadoutChannel.h"

using namespace std;

ClassImp(TRestDetectorReadoutChannel);
///////////////////////////////////////////////
/// \brief TRestDetectorReadoutChannel default constructor
///
TRestDetectorReadoutChannel::TRestDetectorReadoutChannel() { Initialize(); }

///////////////////////////////////////////////
/// \brief TRestDetectorReadoutChannel default destructor
///
TRestDetectorReadoutChannel::~TRestDetectorReadoutChannel() {}

///////////////////////////////////////////////
/// \brief Initializes the channel members
///
void TRestDetectorReadoutChannel::Initialize() { fDaqID = -1; }

///////////////////////////////////////////////
/// \brief Determines if (x,y) referenced to module coordinates is inside the
/// channel
///
Int_t TRestDetectorReadoutChannel::isInside(Double_t x, Double_t y) {
    for (auto& pixel : fReadoutPixel) {
        if (pixel.IsInside({x, y})) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////
/// \brief Prints the details of the readout channel including pixel
/// coordinates.
///
void TRestDetectorReadoutChannel::Print(int DetailLevel) {
    if (DetailLevel >= 0) {
        RESTMetadata << "++++ Channel: " << GetChannelId() << " Daq channel: " << GetDaqID()
                     << " Channel name: " << GetChannelName() << " Channel type: " << GetChannelType()
                     << " total pixels: " << GetNumberOfPixels() << RESTendl;

        RESTMetadata << "+++++++++++++++++++++++++++++++++++++++++++++++++" << RESTendl;

        if (DetailLevel - 1 >= 0) {
            for (int n = 0; n < GetNumberOfPixels(); n++) {
                fReadoutPixel[n].Print();
            }
        }
    }
}
