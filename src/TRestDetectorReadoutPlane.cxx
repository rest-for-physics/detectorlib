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
/// This class stores the readout plane geometrical description, plane
/// position, orientation, and cathode position. It contains
/// a vector of TRestDetectorReadoutModule with the readout modules that are
/// implemented in the readout plane.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-mar:  First concept.
///            Javier Galan
///
/// \class      TRestDetectorReadoutPlane
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestDetectorReadoutPlane.h"

using namespace std;

ClassImp(TRestDetectorReadoutPlane);
///////////////////////////////////////////////
/// \brief Default TRestDetectorReadoutPlane constructor
///
TRestDetectorReadoutPlane::TRestDetectorReadoutPlane() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default TRestDetectorReadoutPlane destructor
///
TRestDetectorReadoutPlane::~TRestDetectorReadoutPlane() {}

///////////////////////////////////////////////
/// \brief TRestDetectorReadoutPlane initialization
///
void TRestDetectorReadoutPlane::Initialize() {
    fPosition = TVector3(0, 0, 0);
    fNormal = TVector3(0, 0, 1);
    fHeight = 0;

    fReadoutModules.clear();
}

///////////////////////////////////////////////
/// \brief Returns the total number of channels in the readout plane
///
Int_t TRestDetectorReadoutPlane::GetNumberOfChannels() {
    Int_t nChannels = 0;
    for (size_t md = 0; md < GetNumberOfModules(); md++) {
        nChannels += fReadoutModules[md].GetNumberOfChannels();
    }
    return nChannels;
}

///////////////////////////////////////////////
/// \brief Returns a pointer to a module using its internal module id
///
TRestDetectorReadoutModule* TRestDetectorReadoutPlane::GetModuleByID(Int_t modID) {
    for (size_t md = 0; md < GetNumberOfModules(); md++)
        if (fReadoutModules[md].GetModuleID() == modID) return &fReadoutModules[md];

    cout << "REST ERROR (GetReadoutModuleByID) : Module ID : " << modID << " was not found" << endl;
    return nullptr;
}

///////////////////////////////////////////////
/// \brief Returns the X coordinate of a given channel in a given module using
/// their internal module and channel ids.
///
/// This method evaluates if the channel is a strip or a pixel. If it is a
/// strip and the X-axis is not localized, this function returns NaN.
///
/// \param modID Internal module id. As defined on the readout.
/// \param chID Internal channel id. As defined on the readout.
///
/// \return The value of the X-coordinate relative to the readout position
Double_t TRestDetectorReadoutPlane::GetX(Int_t modID, Int_t chID) {
    TRestDetectorReadoutModule* rModule = GetModuleByID(modID);

    TRestDetectorReadoutChannel* rChannel = rModule->GetChannel(chID);

    Double_t x = numeric_limits<Double_t>::quiet_NaN();

    if (rChannel->GetNumberOfPixels() == 1) {
        Double_t sX = rChannel->GetPixel(0)->GetSize().X();
        Double_t sY = rChannel->GetPixel(0)->GetSize().Y();

        if (sX > 2 * sY) return x;

        x = rModule->GetPixelCenter(chID, 0).X();

        return x;
    }

    if (rChannel->GetNumberOfPixels() > 1) {
        Int_t nPix = rChannel->GetNumberOfPixels();

        // We check the origin of consecutive pixels to check if it goes X or Y
        // direction. Perhaps more complex readouts need some changes here
        Double_t x1 = rChannel->GetPixel(0)->GetCenter().X();
        Double_t x2 = rChannel->GetPixel(nPix - 1)->GetCenter().X();

        Double_t y1 = rChannel->GetPixel(0)->GetCenter().Y();
        Double_t y2 = rChannel->GetPixel(nPix - 1)->GetCenter().Y();

        /*
        cout << "Pixel 0. X : " << x1 << " Y : " << y1 endl;
        cout << "Pixel N. X : " << x2 << " Y : " << y2 endl;
        */

        Double_t deltaX = abs(x2 - x1);
        Double_t deltaY = abs(y2 - y1);

        Int_t rotation = (Int_t)rModule->GetModuleRotation();
        if (rotation % 90 == 0) {
            if (rotation / 90 % 2 == 0)  // rotation is 0, 180, 360...
            {
                if (deltaY > deltaX) x = rModule->GetPixelCenter(chID, 0).X();
            } else  // rotation is 90, 270... We need to invert x and y
            {
                if (deltaY < deltaX) x = rModule->GetPixelCenter(chID, 0).X();
            }
        } else {
            // we choose to output x only when deltaY > deltaX under non-90 deg rotation
            // otherwise it is a y channel and should return nan
            if (deltaY > deltaX) x = rModule->GetPixelCenter(chID, 0).X();
        }
    }

    return x;
}

///////////////////////////////////////////////
/// \brief Returns the Y coordinate of a given channel in a given module using
/// their internal module and channel ids.
///
/// This method evaluates if the channel is a strip or a pixel. If it is a
/// strip and the Y-axis is not localized, this function returns NaN.
///
/// \param modID Internal module id. As defined on the readout.
/// \param chID Internal channel id. As defined on the readout.
///
/// \return The value of the X-coordinate relative to the readout position
Double_t TRestDetectorReadoutPlane::GetY(Int_t modID, Int_t chID) {
    TRestDetectorReadoutModule* rModule = GetModuleByID(modID);

    TRestDetectorReadoutChannel* rChannel = rModule->GetChannel(chID);

    Double_t y = numeric_limits<Double_t>::quiet_NaN();

    if (rChannel->GetNumberOfPixels() == 1) {
        Double_t sX = rChannel->GetPixel(0)->GetSize().X();
        Double_t sY = rChannel->GetPixel(0)->GetSize().Y();

        if (sY > 2 * sX) return y;

        y = rModule->GetPixelCenter(chID, 0).Y();

        return y;
    }

    if (rChannel->GetNumberOfPixels() > 1) {
        Int_t nPix = rChannel->GetNumberOfPixels();

        // We check the origin of consecutive pixels to check if it goes X or Y
        // direction. Perhaps more complex readouts need some changes here
        Double_t x1 = rChannel->GetPixel(0)->GetCenter().X();
        Double_t x2 = rChannel->GetPixel(nPix - 1)->GetCenter().X();

        Double_t y1 = rChannel->GetPixel(0)->GetCenter().Y();
        Double_t y2 = rChannel->GetPixel(nPix - 1)->GetCenter().Y();

        /*
           cout << "Pix id : " << rChannel->GetPixel(0)->GetID() << " X1 : " << x1
           << endl; cout << "Pix id : " << rChannel->GetPixel(1)->GetID() << " X2 :
           " << x2 << endl; cout << "Pix id : " << rChannel->GetPixel(0)->GetID() <<
           " Y1 : " << y1 << endl; cout << "Pix id : " <<
           rChannel->GetPixel(1)->GetID() << " Y2 : " << y2 << endl;
           */

        Double_t deltaX = abs(x2 - x1);
        Double_t deltaY = abs(y2 - y1);

        Int_t rotation = (Int_t)rModule->GetModuleRotation();
        if (rotation % 90 == 0) {
            if (rotation / 90 % 2 == 0)  // rotation is 0, 180, 360...
            {
                if (deltaY < deltaX) y = rModule->GetPixelCenter(chID, 0).Y();
            } else  // rotation is 90, 270... We need to invert x and y
            {
                if (deltaY > deltaX) y = rModule->GetPixelCenter(chID, 0).Y();
            }
        } else {
            // we choose to output y only when deltaY < deltaX under non-90 deg rotation
            // otherwise it is a x channel and should return nan
            if (deltaY < deltaX) y = rModule->GetPixelCenter(chID, 0).Y();
        }
    }

    return y;
}

///////////////////////////////////////////////
/// \brief Finds the readout channel index for a given module stored in a given
/// module index stored in the readout plane (internal readout plane module id).
///
/// \param absX It is the x absolut physical position
/// \param absY It is the y absolut physical position
/// \return The corresponding channel id
Int_t TRestDetectorReadoutPlane::FindChannel(Int_t module, const TVector2& position) {
    const auto relativePosition = position - TVector2{fPosition.X(), fPosition.Y()};

    // TODO : check first if (modX,modY) is inside the module.
    // If not return error.
    // FindChannel will take a long time to search for the channel if it is not
    // there. It will be faster

    return fReadoutModules[module].FindChannel(relativePosition);
}

///////////////////////////////////////////////
/// \brief Returns the perpendicular distance to the readout plane of a given
/// *x*, *y*, *z* position
///
Double_t TRestDetectorReadoutPlane::GetDistanceTo(Double_t x, Double_t y, Double_t z) {
    return GetDistanceTo(TVector3(x, y, z));
}

///////////////////////////////////////////////
/// \brief Returns the perpendicular distance to the readout plane of a given
/// TVector3 position
///
Double_t TRestDetectorReadoutPlane::GetDistanceTo(const TVector3& pos) {
    return (pos - GetPosition()).Dot(GetNormal());
}

void TRestDetectorReadoutPlane::SetHeight(Double_t height) {
    if (height < 0) {
        RESTError << "TRestDetectorReadoutPlane::SetHeight : height cannot be negative." << RESTendl;
        exit(1);
    }
    fHeight = height;
}
///////////////////////////////////////////////
/// \brief This method determines if a given position in *z* is inside the drift
/// volume drifting distance for this readout plane.
///
/// \return 1 if the Z-position is found inside the drift volume definition. 0
/// otherwise returns -1.
///
Int_t TRestDetectorReadoutPlane::isZInsideDriftVolume(Double_t z) {
    TVector3 pos = TVector3(0, 0, z);

    return isZInsideDriftVolume(pos);
}

///////////////////////////////////////////////
/// \brief This method determines if the daqId given is associated to any of the
/// readout readout channels in any readout modules.
///
/// \return true if daqId is found
/// returns false if daqId is not found
///
Bool_t TRestDetectorReadoutPlane::isDaqIDInside(Int_t daqId) {
    for (size_t m = 0; m < GetNumberOfModules(); m++)
        if (fReadoutModules[m].isDaqIDInside(daqId)) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief This method determines if the z-coordinate is inside the drift volume
/// for this readout plane.
///
/// \param position A TVector3 defining the position.
///
/// \return 1 if the Z-position is found inside the drift volume definition. 0
/// otherwise
///
Int_t TRestDetectorReadoutPlane::isZInsideDriftVolume(const TVector3& position) {
    TVector3 posNew = TVector3(position.X() - fPosition.X(), position.Y() - fPosition.Y(), position.Z());

    Double_t distance = GetDistanceTo(posNew);

    if (distance > 0 && distance < fHeight) return 1;

    return 0;
}

///////////////////////////////////////////////
/// \brief This method returns the module id where the hits with coordinates
/// (x,y,z) is found. The z-coordinate must be found in between the cathode and
/// the readout plane. The *x* and *y* values must be found inside one of the
/// readout modules defined inside the readout plane.
///
/// \param x,y,z Three Double_t defining the position.
///
/// \return the module *id* where the hit is found. If no module *id* is found
/// it returns -1.
///
Int_t TRestDetectorReadoutPlane::GetModuleIDFromPosition(Double_t x, Double_t y, Double_t z) {
    TVector3 pos = TVector3(x, y, z);

    return GetModuleIDFromPosition(pos);
}
///////////////////////////////////////////////
/// \brief This method returns the module id where *pos* is found.
/// The z-coordinate must be found in between
/// the cathode and the readout plane. The *x* and *y* values must be found
/// inside one of the readout modules defined inside the readout plane.
///
/// \param pos A TVector3 defining the position.
///
/// \return the module *id* where the hit is found. If no module *id* is found
/// it returns -1.
///
Int_t TRestDetectorReadoutPlane::GetModuleIDFromPosition(TVector3 pos) {
    TVector3 posNew = TVector3(pos.X() - fPosition.X(), pos.Y() - fPosition.Y(), pos.Z());

    Double_t distance = GetDistanceTo(posNew);

    if (distance > 0 && distance < fHeight) {
        for (size_t m = 0; m < GetNumberOfModules(); m++) {
            if (fReadoutModules[m].isInside(posNew.X(), posNew.Y())) {
                return fReadoutModules[m].GetModuleID();
            }
        }
    }

    return -1;
}

///////////////////////////////////////////////
/// \brief Prints information with details of the readout plane and modules
/// defined inside the readout plane.
///
void TRestDetectorReadoutPlane::Print(Int_t DetailLevel) {
    if (DetailLevel >= 0) {
        RESTMetadata << "-- Readout plane : " << GetID() << RESTendl;
        RESTMetadata << "----------------------------------------------------------------" << RESTendl;
        RESTMetadata << "-- Position : X = " << fPosition.X() << " mm, "
                     << " Y : " << fPosition.Y() << " mm, Z : " << fPosition.Z() << " mm" << RESTendl;
        RESTMetadata << "-- Normal vector : X = " << fNormal.X() << " mm, "
                     << " Y : " << fNormal.Y() << " mm, Z : " << fNormal.Z() << " mm" << RESTendl;
        RESTMetadata << "-- X-axis vector : X = " << fAxisX.X() << " mm, "
                     << " Y : " << fAxisX.Y() << " mm, Z : " << fAxisX.Z() << " mm" << RESTendl;
        RESTMetadata << "-- Y-axis vector : Y = " << fAxisY.X() << " mm, "
                     << " Y : " << fAxisY.Y() << " mm, Z : " << fAxisY.Z() << " mm" << RESTendl;
        const TVector3 cathodePosition = GetCathodePosition();
        RESTMetadata << "-- Cathode Position : X = " << cathodePosition.X() << " mm, "
                     << " Y : " << cathodePosition.Y() << " mm, Z : " << cathodePosition.Z() << " mm"
                     << RESTendl;
        RESTMetadata << "-- Total drift distance : " << fHeight << " mm" << RESTendl;
        RESTMetadata << "-- Charge collection : " << fChargeCollection << RESTendl;
        RESTMetadata << "-- Total modules : " << GetNumberOfModules() << RESTendl;
        RESTMetadata << "-- Total channels : " << GetNumberOfChannels() << RESTendl;
        RESTMetadata << "----------------------------------------------------------------" << RESTendl;

        for (size_t i = 0; i < GetNumberOfModules(); i++) {
            fReadoutModules[i].Print(DetailLevel - 1);
        }
    }
}

///////////////////////////////////////////////
/// \brief Draws the readout plane using GetReadoutHistogram.
///
void TRestDetectorReadoutPlane::Draw() { this->GetReadoutHistogram()->Draw(); }

///////////////////////////////////////////////
/// \brief Creates and returns a TH2Poly object with the
/// readout pixel description.
///
TH2Poly* TRestDetectorReadoutPlane::GetReadoutHistogram() {
    Double_t x[4];
    Double_t y[4];

    double xmin, xmax, ymin, ymax;

    GetBoundaries(xmin, xmax, ymin, ymax);

    TH2Poly* readoutHistogram = new TH2Poly("ReadoutHistogram", "ReadoutHistogram", xmin, xmax, ymin, ymax);

    for (size_t mdID = 0; mdID < this->GetNumberOfModules(); mdID++) {
        TRestDetectorReadoutModule* module = &fReadoutModules[mdID];

        int nChannels = module->GetNumberOfChannels();

        for (int ch = 0; ch < nChannels; ch++) {
            TRestDetectorReadoutChannel* channel = module->GetChannel(ch);
            Int_t nPixels = channel->GetNumberOfPixels();

            for (int px = 0; px < nPixels; px++) {
                for (int v = 0; v < 4; v++) {
                    x[v] = module->GetPixelVertex(ch, px, v).X();
                    y[v] = module->GetPixelVertex(ch, px, v).Y();
                }

                readoutHistogram->AddBin(4, x, y);
            }
        }
    }

    readoutHistogram->SetStats(0);

    return readoutHistogram;
}

///////////////////////////////////////////////
/// \brief Finds the xy boundaries of the readout plane delimited by the modules
///
void TRestDetectorReadoutPlane::GetBoundaries(double& xmin, double& xmax, double& ymin, double& ymax) {
    Double_t x[4];
    Double_t y[4];

    xmin = 1E9, xmax = -1E9, ymin = 1E9, ymax = -1E9;

    for (size_t mdID = 0; mdID < this->GetNumberOfModules(); mdID++) {
        TRestDetectorReadoutModule* module = &fReadoutModules[mdID];

        for (int v = 0; v < 4; v++) {
            x[v] = module->GetVertex(v).X();
            y[v] = module->GetVertex(v).Y();

            if (x[v] < xmin) xmin = x[v];
            if (y[v] < ymin) ymin = y[v];
            if (x[v] > xmax) xmax = x[v];
            if (y[v] > ymax) ymax = y[v];
        }
    }
}
