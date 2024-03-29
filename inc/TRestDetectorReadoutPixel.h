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

#ifndef RestCore_TRestDetectorReadoutPixel
#define RestCore_TRestDetectorReadoutPixel

#include <TMath.h>
#include <TVector2.h>

#include <iostream>

/// A class to store the readout pixel definition used in TRestDetectorReadoutChannel.
class TRestDetectorReadoutPixel {
   private:
    // TODO: refactor as TVector2 as with other readout classes (readouts will need to be regenerated)

    Double_t fPixelOriginX = 0;  ///< The pixel x-origin position, left-bottom corner.
    Double_t fPixelOriginY = 0;  ///< The pixel y-origin position, left-bottom corner.

    Double_t fPixelSizeX = 0;  ///< The pixel x size.
    Double_t fPixelSizeY = 0;  ///< The pixel y size.

    Double_t fRotation = 0;  ///< The pixel rotation angle in degrees, rotation with
                             ///< axis at the origin position.

    Bool_t fTriangle = false;  ///< The type of the pixel : false is rectangular, true is
                               ///< triangle

    /// It will be initialized with the module parameter "pixelTolerance"
    Double_t fTolerance = 1.e-6;  //!

    void Initialize();

   protected:
   public:
    /// Returns the x-coordinate pixel origin.
    Double_t GetOriginX() const { return fPixelOriginX; }

    /// Returns the y-coordinate pixel origin.
    Double_t GetOriginY() const { return fPixelOriginY; }

    /// Returns a TVector2 with the pixel origin.
    TVector2 GetOrigin() const { return {fPixelOriginX, fPixelOriginY}; }

    /// Returns the rotation angle in degrees
    Double_t GetRotation() const { return fRotation; }

    /// Returns the pixel size in X
    inline Double_t GetSizeX() const { return fPixelSizeX; }

    /// Returns the pixel size in Y
    inline Double_t GetSizeY() const { return fPixelSizeY; }

    /// Returns a TVector2 with the pixel size.
    TVector2 GetSize() { return {fPixelSizeX, fPixelSizeY}; }

    /// Returns true if the pixel is a triangle.
    Bool_t GetTriangle() const { return fTriangle; }

    TVector2 GetCenter() const;

    TVector2 GetVertex(int n) const;

    /// Sets the origin of the pixel using a TVector2.
    void SetOrigin(const TVector2& origin) {
        fPixelOriginX = origin.X();
        fPixelOriginY = origin.Y();
    }

    /// Sets the size of the pixel using a TVector2.
    void SetSize(const TVector2& size) {
        fPixelSizeX = size.X();
        fPixelSizeY = size.Y();
    }

    /// Sets the rotation angle of the pixel in degrees
    void SetRotation(Double_t rot) { fRotation = rot; }

    /// Sets the type of the pixel
    void SetTriangle(Bool_t type) { fTriangle = type; }

    /// Sets the value of the tolerance in mm. Used in IsInside method.
    void SetTolerance(Double_t tol) { fTolerance = tol; }

    Bool_t IsInside(const TVector2& pos);

    TVector2 TransformToPixelCoordinates(const TVector2& pixel) const;

    void Print() const;

    // Constructor
    TRestDetectorReadoutPixel();
    // Destructor
    virtual ~TRestDetectorReadoutPixel();

    ClassDef(TRestDetectorReadoutPixel, 5);
};

#endif
