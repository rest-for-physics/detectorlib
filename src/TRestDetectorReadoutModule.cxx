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
/// This class stores the readout module geometrical description, module
/// position, orientation, and size. It contains a vector of
/// TRestDetectorReadoutChannel with the definition of the readout channels
/// existing in the readout module.
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
/// \class      TRestDetectorReadoutModule
/// \author     Javier Galan
///
/// <hr>
///

#ifdef __APPLE__
#include <unistd.h>
#endif

#include "TRestDetectorReadoutModule.h"

using namespace std;

ClassImp(TRestDetectorReadoutModule);
///////////////////////////////////////////////
/// \brief Default TRestDetectorReadoutModule constructor
///
TRestDetectorReadoutModule::TRestDetectorReadoutModule() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default TRestDetectorReadoutModule destructor
///
TRestDetectorReadoutModule::~TRestDetectorReadoutModule() {}

///////////////////////////////////////////////
/// \brief TRestDetectorReadoutModule initialization
///
void TRestDetectorReadoutModule::Initialize() {
    fReadoutChannel.clear();
    fModuleID = -1;

    fModuleOrigin = {0, 0};
    fModuleSize = {0, 0};

    fModuleRotation = 0;

    fMaximumDaqId = -1;
    fMinimumDaqId = -1;

    fTolerance = 1.e-3;

    showWarnings = false;
}

///////////////////////////////////////////////
/// \brief Initializes the max and min values for the daq channel number
///
void TRestDetectorReadoutModule::SetMinMaxDaqIDs() {
    Int_t maxID = GetChannel(0)->GetDaqID();
    Int_t minID = GetChannel(0)->GetDaqID();
    for (auto ch = 0; ch < this->GetNumberOfChannels(); ch++) {
        Int_t daqID = GetChannel(ch)->GetDaqID();
        if (daqID > maxID) maxID = daqID;

        if (daqID < minID) minID = daqID;
    }

    fMaximumDaqId = maxID;
    fMinimumDaqId = minID;
}

///////////////////////////////////////////////
/// \brief Starts the readout mapping initialization. This process
/// is computationally expensive but it greatly optimizes the FindChannel
/// process later on.
///
void TRestDetectorReadoutModule::DoReadoutMapping(Int_t nodes) {
    ///////////////////////////////////////////////////////////////////////////////
    // We initialize the mapping readout net to sqrt(numberOfPixels)
    // However this might not be good for readouts where the pixels are
    // assymmetric
    // /////////////////////////////////////////////////////////////////////////////
    Int_t totalNumberOfPixels = 0;
    for (auto ch = 0; ch < this->GetNumberOfChannels(); ch++) {
        totalNumberOfPixels += fReadoutChannel[ch].GetNumberOfPixels();
    }
    if (nodes == 0) {
        nodes = TMath::Sqrt(totalNumberOfPixels);
        nodes = 2 * nodes;
    }

    cout << "Performing readout mapping optimization (This might require long "
            "computation time)"
         << endl;
    cout << "--------------------------------------------------------------------"
            "--------------"
         << endl;
    cout << "Total number of pixels : " << totalNumberOfPixels << endl;
    cout << "Nodes : " << nodes << endl;

    fMapping.Initialize(nodes, nodes, GetModuleSizeX(), GetModuleSizeY());

    for (auto ch = 0; ch < this->GetNumberOfChannels(); ch++) {
        for (int px = 0; px < fReadoutChannel[ch].GetNumberOfPixels(); px++) {
            Double_t xPix = fReadoutChannel[ch].GetPixel(px)->GetCenter().X();
            Double_t yPix = fReadoutChannel[ch].GetPixel(px)->GetCenter().Y();

            Int_t nodeX = fMapping.GetNodeX(xPix);
            Int_t nodeY = fMapping.GetNodeY(yPix);

            // This means that two pixels in the readout are associated to the same
            // node.
            // If the granularity of the readout is not high enough this may happen
            // often. This should be just a warning I guess.
            if (showWarnings && fMapping.isNodeSet(nodeX, nodeY)) {
                cout << endl;
                cout << "TRestDetectorReadoutModule. WARNING. Node is already SET!!" << endl;
                cout << "Trying to associate channel : " << ch << " Pixel : " << px << endl;
                cout << "Pixel coordinates : ( " << xPix << " , " << yPix << " ) " << endl;

                Int_t tempCh = fMapping.GetChannelByNode(nodeX, nodeY);
                Int_t tempPix = fMapping.GetPixelByNode(nodeX, nodeY);
                RESTWarning << "Already associated channel : " << tempCh << " pixel : " << tempPix
                            << RESTendl;
                Double_t xP = fReadoutChannel[tempCh].GetPixel(tempPix)->GetCenter().X();
                Double_t yP = fReadoutChannel[tempCh].GetPixel(tempPix)->GetCenter().Y();
                RESTWarning << "Pixel coordinates : ( " << xP << " , " << yP << " ) " << RESTendl;
                cout << endl;

                cout << "Increasing the number of mapping of nodes may solve this issue." << endl;
                cout << endl;
            }
            fMapping.SetNode(nodeX, nodeY, ch, px);
        }
    }

    for (auto i = 0; i < nodes; i++) {
        printf("Completed : %.2lf %%\r", 100. * (i * (Double_t)nodes) / nodes / nodes);
        fflush(stdout);
        for (auto j = 0; j < nodes; j++) {
            Double_t x = fMapping.GetX(i);
            Double_t y = fMapping.GetY(j);
            const auto transformedCoordinates = TransformToPhysicalCoordinates(x, y);

            if (!fMapping.isNodeSet(i, j)) {
                for (auto ch = 0; ch < GetNumberOfChannels() && !fMapping.isNodeSet(i, j); ch++) {
                    for (int px = 0;
                         px < fReadoutChannel[ch].GetNumberOfPixels() && !fMapping.isNodeSet(i, j); px++) {
                        if (isInsidePixel(ch, px, transformedCoordinates)) {
                            fMapping.SetNode(i, j, ch, px);
                        }
                    }
                }
            }
        }
    }

    if (!fMapping.AllNodesSet())
        cout << "Not all nodes set" << endl;
    else
        cout << "All Nodes set" << endl;

    for (auto i = 0; i < nodes; i++)
        for (auto j = 0; j < nodes; j++) {
            if (!fMapping.isNodeSet(i, j)) {
                Double_t x = fMapping.GetX(i);
                Double_t y = fMapping.GetY(j);
                const auto transformedCoordinates = TransformToPhysicalCoordinates(x, y);

                cout << "Node NOT SET : " << i << " , " << j << " Mapping x : " << x << " y : " << y << endl;

                for (auto ch = 0; ch < GetNumberOfChannels(); ch++) {
                    for (int px = 0; px < fReadoutChannel[ch].GetNumberOfPixels(); px++) {
                        if (isInsidePixel(ch, px, transformedCoordinates)) {
                            cout << "X : " << transformedCoordinates.X() << " , "
                                 << transformedCoordinates.Y() << " Is inside channel : " << ch
                                 << " pixel : " << px << endl;
                        }
                    }
                }
            }
        }

    cout << "Nodes not set : " << fMapping.GetNumberOfNodesNotSet() << endl;
}

///////////////////////////////////////////////
/// \brief Determines if a given *daqID* number is in the range of the module
///
Bool_t TRestDetectorReadoutModule::isDaqIDInside(Int_t daqID) const {
    if (daqID >= fMinimumDaqId && daqID <= fMaximumDaqId) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief Returns the channel index corresponding to the absolute coordinates
/// (absX, absY), but relative to the readout plane coordinate system.
///
/// The readout mapping (see TRestDetectorReadoutMapping) is used to help finding
/// the pixel where coordinates absX and absY fall in.
///
Int_t TRestDetectorReadoutModule::FindChannel(const TVector2& position) const {
    if (!isInside(position)) return -1;

    const auto transformedCoordinates = TransformToModuleCoordinates(position);
    const auto& x = transformedCoordinates.X();
    const auto& y = transformedCoordinates.Y();

    Int_t nodeX = fMapping.GetNodeX(x);
    Int_t nodeY = fMapping.GetNodeY(y);

    Int_t channel = fMapping.GetChannelByNode(nodeX, nodeY);
    Int_t pixel = fMapping.GetPixelByNode(nodeX, nodeY);

    Int_t repeat = 1;
    Int_t count = 0;
    Int_t forward = 1;
    Int_t xAxis = 1;

    Int_t totalNodes = fMapping.GetNumberOfNodesX() * fMapping.GetNumberOfNodesY();

    // We test if x,y is inside the channel/pixel obtained from the readout
    // mapping If not we start to look in the readout mapping neighbours
    while (!this->isInsidePixel(channel, pixel, position)) {
        count++;
        if (xAxis == 1 && forward == 1)
            nodeX++;
        else if (xAxis == 0 && forward == 1)
            nodeY++;
        else if (xAxis == 1 && forward == 0)
            nodeX--;
        else if (xAxis == 0 && forward == 0)
            nodeY--;

        Int_t nNodes = fMapping.GetNumberOfNodesX();

        if (nodeX < 0) nodeX = nNodes - 1;
        if (nodeY < 0) nodeY = nNodes - 1;
        if (nodeX >= nNodes) nodeX = 0;
        if (nodeY >= nNodes) nodeY = 0;

        if (count >= repeat) {
            if (xAxis == 1 && forward == 1) {
                xAxis = 0;
                forward = 0;
            } else if (xAxis == 0 && forward == 0) {
                xAxis = 1;
                forward = 0;
                repeat++;
            } else if (xAxis == 1 && forward == 0) {
                xAxis = 0;
                forward = 1;
            } else if (xAxis == 0 && forward == 1) {
                xAxis = 1;
                forward = 1;
                repeat++;
            }

            count = 0;
        }

        channel = fMapping.GetChannelByNode(nodeX, nodeY);
        pixel = fMapping.GetPixelByNode(nodeX, nodeY);

        if (count > totalNodes / 10) {
            RESTWarning << "TRestDetectorReadoutModule. I did not find any channel for hit position (" << x
                        << "," << y << ") in internal module coordinates" << RESTendl;

            for (auto ch = 0; ch < GetNumberOfChannels(); ch++)
                for (auto px = 0; px < fReadoutChannel[ch].GetNumberOfPixels(); px++)
                    if (isInsidePixel(ch, px, position)) {
                        cout << "( " << x << " , " << y << ") Should be in channel " << ch
                             << " pixel : " << px << endl;

                        cout << "Corresponding node :  nX: " << fMapping.GetNodeX_ForChannelAndPixel(ch, px)
                             << " nY : " << fMapping.GetNodeY_ForChannelAndPixel(ch, px) << endl;
                        cout << "Channel : " << ch << " Pixel : " << px << endl;
                        cout << "Pix X : " << GetChannel(ch)->GetPixel(px)->GetCenter().X()
                             << " Pix Y : " << GetChannel(ch)->GetPixel(px)->GetCenter().Y() << endl;
                    }
            sleep(5);
            return -1;
        }
    }

    return channel;
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* relative to the readout
/// plane are inside this readout module.
///
Bool_t TRestDetectorReadoutModule::isInside(const TVector2& position) const {
    TVector2 positionRotated = TransformToModuleCoordinates(position);

    if (positionRotated.X() >= 0 && positionRotated.X() <= fModuleSize.X() && positionRotated.Y() >= 0 &&
        positionRotated.Y() <= fModuleSize.Y())
        return true;

    return false;
}

///////////////////////////////////////////////
/// \brief Determines if the position *x,y* is found in any of the pixels
/// of the readout *channel* index given.
///
Bool_t TRestDetectorReadoutModule::isInsideChannel(Int_t channel, Double_t x, Double_t y) const {
    return isInsideChannel(channel, {x, y});
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* is found in any of the
/// pixels of the readout *channel* index given.
///
Bool_t TRestDetectorReadoutModule::isInsideChannel(Int_t channel, const TVector2& position) const {
    TVector2 pos = TransformToModuleCoordinates(position);
    for (int idx = 0; idx < GetChannel(channel)->GetNumberOfPixels(); idx++)
        if (GetChannel(channel)->GetPixel(idx)->isInside(pos)) {
            return true;
        }
    return false;
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* is found at a specific
/// *pixel* id inside the readout *channel* given.
///
Bool_t TRestDetectorReadoutModule::isInsidePixel(Int_t channel, Int_t pixel, const TVector2& position) const {
    if (channel < 0 || pixel < 0) return false;

    TVector2 pos = TransformToModuleCoordinates(position);
    if (GetChannel(channel)->GetPixel(pixel)->isInside(pos)) {
        return true;
    }
    return false;
}

///////////////////////////////////////////////
/// \brief Creates a TVector2 with shortest norm going from the given position
/// *pos* to the module.
///  It can be seen as the vector to add to move from the position to the
///  closest border of the module.
///
TVector2 TRestDetectorReadoutModule::GetDistanceToModule(const TVector2& position) const {
    TVector2 newPos = TransformToModuleCoordinates(position);

    Double_t dx = 0, dy = 0;
    if (newPos.X() < 0)
        dx = -newPos.X();
    else if (fModuleSize.X() - newPos.X() < 0)
        dx = fModuleSize.X() - newPos.X();

    if (newPos.Y() < 0)
        dy = -newPos.Y();
    else if (fModuleSize.Y() - newPos.Y() < 0)
        dy = fModuleSize.Y() - newPos.Y();

    TVector2 dist = TVector2(dx, dy);
    return dist;
}
///////////////////////////////////////////////
/// \brief Returns the pixel origin (left-bottom) position for a given *channel*
/// and *pixel* indexes.
///
TVector2 TRestDetectorReadoutModule::GetPixelOrigin(Int_t channel, Int_t pixel) const {
    return GetPixelVertex(channel, pixel, 0);
}

///////////////////////////////////////////////
/// \brief Returns any of the pixel vertex position for a given *channel* and
/// *pixel* indexes.
///
/// \param vertex A value between 0-3 defining the vertex position to be
/// returned
///
TVector2 TRestDetectorReadoutModule::GetPixelVertex(Int_t channel, Int_t pixel, Int_t vertex) const {
    TVector2 pixPosition = GetChannel(channel)->GetPixel(pixel)->GetVertex(vertex);

    pixPosition = pixPosition.Rotate(fModuleRotation * TMath::Pi() / 180.);
    pixPosition = pixPosition + fModuleOrigin;
    return pixPosition;
}

///////////////////////////////////////////////
/// \brief Returns the center pixel position for a given *channel* and
/// *pixel* indexes.
///
/// \param vertex A value between 0-3 defining the vertex position to be
/// returned
///
TVector2 TRestDetectorReadoutModule::GetPixelCenter(Int_t channel, Int_t pixel) const {
    TVector2 pixCenter = GetChannel(channel)->GetPixel(pixel)->GetCenter();

    pixCenter = pixCenter.Rotate(fModuleRotation * TMath::Pi() / 180.);
    pixCenter = pixCenter + fModuleOrigin;
    return pixCenter;
}

///////////////////////////////////////////////
/// \brief Returns the pixel type for a given *channel* and
/// *pixel* indexes.
///
/// \param vertex A boolean that is true if the pixel is triangular, false
/// otherwise
///
Bool_t TRestDetectorReadoutModule::GetPixelTriangle(Int_t channel, Int_t pixel) const {
    Bool_t type = GetChannel(channel)->GetPixel(pixel)->GetTriangle();

    return type;
}

TVector2 TRestDetectorReadoutModule::GetPixelOrigin(TRestDetectorReadoutPixel* pix) const {
    return GetPixelVertex(pix, 0);
}

TVector2 TRestDetectorReadoutModule::GetPixelVertex(TRestDetectorReadoutPixel* pix, Int_t vertex) const {
    TVector2 pixPosition = pix->GetVertex(vertex);
    pixPosition = pixPosition.Rotate(fModuleRotation * TMath::Pi() / 180.);
    pixPosition = pixPosition + fModuleOrigin;
    return pixPosition;
}

TVector2 TRestDetectorReadoutModule::GetPixelCenter(TRestDetectorReadoutPixel* pix) const {
    TVector2 corner1(GetPixelVertex(pix, 0));
    TVector2 corner2(GetPixelVertex(pix, 2));
    TVector2 center = (corner1 + corner2) / 2.;
    return center;
}

Bool_t TRestDetectorReadoutModule::GetPixelTriangle(TRestDetectorReadoutPixel* pix) const {
    return pix->GetTriangle();
}

///////////////////////////////////////////////
/// \brief Returns the coordinates of the specified vertex index *n*. The
/// physical coordinates relative to the readout plane are returned, including
/// rotation.
///
/// \param n A value between 0-3 defining the vertex position to be returned
///
TVector2 TRestDetectorReadoutModule::GetVertex(int n) const {
    TVector2 vertex(0, 0);
    const TVector2& origin = fModuleOrigin;

    if (n % 4 == 0)
        return origin;
    else if (n % 4 == 1) {
        vertex.Set(fModuleSize.X(), 0);
        vertex = vertex.Rotate(fModuleRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    } else if (n % 4 == 2) {
        vertex.Set(fModuleSize.X(), fModuleSize.Y());
        vertex = vertex.Rotate(fModuleRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    } else if (n % 4 == 3) {
        vertex.Set(0, fModuleSize.Y());
        vertex = vertex.Rotate(fModuleRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    }
    return vertex;
}

///////////////////////////////////////////////
/// \brief Adds a new channel to the module
///
void TRestDetectorReadoutModule::AddChannel(TRestDetectorReadoutChannel& rChannel) {
    for (auto i = 0; i < rChannel.GetNumberOfPixels(); i++) {
        // TODO we expect here that the user will only do pixel rotations between 0
        // and 90 degrees, we must force that on pixel definition or fix it here
        Double_t oX = rChannel.GetPixel(i)->GetVertex(3).X();
        Double_t oY = rChannel.GetPixel(i)->GetVertex(3).Y();
        Double_t sX = rChannel.GetPixel(i)->GetVertex(1).X();
        Double_t sY = rChannel.GetPixel(i)->GetVertex(1).Y();

        if (oX + fTolerance < 0 || oY + fTolerance < 0 || sX - fTolerance > fModuleSize.X() ||
            sY - fTolerance > fModuleSize.Y()) {
            if (showWarnings) {
                cout << "REST Warning (AddChannel) pixel outside the module boundaries" << endl;
                cout << "Channel: " << fReadoutChannel.size() << ", Pixel : " << i << endl;
                cout << "Pixel origin = (" << oX << " , " << oY << ")" << endl;
                cout << "Pixel size = (" << sX << " , " << sY << ")" << endl;
                cout << "Module size = (" << fModuleSize.X() << " , " << fModuleSize.Y() << ")" << endl;
            }
        }
    }

    fReadoutChannel.push_back(rChannel);
}

///////////////////////////////////////////////
/// \brief Not implemented
///
void TRestDetectorReadoutModule::Draw() {}

///////////////////////////////////////////////
/// \brief Prints the module details and channels if *fullDetail* is enabled.
///
void TRestDetectorReadoutModule::Print(Int_t DetailLevel) {
    if (DetailLevel >= 0) {
        RESTMetadata << "-- Readout module : " << GetModuleID() << RESTendl;
        RESTMetadata << "----------------------------------------------------------------" << RESTendl;
        RESTMetadata << "-- Origin position : X = " << fModuleOrigin.X() << " mm "
                     << " Y : " << fModuleOrigin.Y() << " mm" << RESTendl;
        RESTMetadata << "-- Size : X = " << fModuleSize.X() << " Y : " << fModuleSize.Y() << RESTendl;
        RESTMetadata << "-- Rotation : " << fModuleRotation << " degrees" << RESTendl;
        RESTMetadata << "-- Total channels : " << GetNumberOfChannels() << RESTendl;
        RESTMetadata << "-- Tolerance : " << fTolerance << RESTendl;
        RESTMetadata << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << RESTendl;

        for (auto n = 0; n < GetNumberOfChannels(); n++) {
            fReadoutChannel[n].Print(DetailLevel - 1);
        }
    }
}
