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

#ifndef RestCore_TRestDetectorReadoutModule
#define RestCore_TRestDetectorReadoutModule

#include <TMath.h>
#include <TObject.h>
#include <TVector2.h>

#include <iostream>

#include "TRestDetectorReadoutChannel.h"
#include "TRestDetectorReadoutMapping.h"

/// A class to store the readout module definition used in TRestDetectorReadoutPlane. It
/// allows to integrate any number of independent readout channels.
class TRestDetectorReadoutModule : public TObject {
   private:
    Int_t fModuleID;  ///< The module id given by the readout definition.

    TString fModuleName;  ///< The assigned module name.

    Double_t fModuleOriginX;  ///< The module x-position (left-bottom corner)
                              ///< relative to the readout plane position.
    Double_t fModuleOriginY;  ///< The module y-position (left-bottom corner)
                              ///< relative to the readout plane position.

    Double_t fModuleSizeX;  ///< X-size of the module. All pixels should be
                            ///< containned within this size.
    Double_t fModuleSizeY;  ///< Y-size of the module. All pixels should be
                            ///< containned within this size.

    Double_t fModuleRotation;  ///< The rotation of the module around the
                               ///< position=(fModuleOriginX, fModuleOriginY) in
                               ///< degrees.

    Int_t fMininimumDaqId;  ///< The minimum daq channel id associated to the
                            ///< module.
    Int_t fMaximumDaqId;    ///< The maximum daq channel id associated to the module.

    std::vector<TRestDetectorReadoutChannel>
        fReadoutChannel;  ///< A std::vector of the instances of TRestDetectorReadoutChannel
                          ///< containned in the readout module.

    TRestDetectorReadoutMapping fMapping;  ///< The readout module uniform grid mapping.

    Double_t fTolerance;  ///< Tolerance allowed in overlaps at the pixel
                          ///< boundaries in mm.

#ifndef __CINT__
    Bool_t showWarnings;  //!///< Flag to enable/disable warning outputs. Disabled by
                          //! default. REST_Warning in TRestDetectorReadout will enable it.
#endif

    void Initialize();

    /// Converts the coordinates given by TVector2 in the readout plane reference
    /// system to the readout module reference system.
    TVector2 TransformToModuleCoordinates(TVector2 p) { return TransformToModuleCoordinates(p.X(), p.Y()); }

    /// Converts the coordinates (xPhys,yPhys) in the readout plane reference
    /// system to the readout module reference system.
    TVector2 TransformToModuleCoordinates(Double_t xPhys, Double_t yPhys) {
        TVector2 coords(xPhys - fModuleOriginX, yPhys - fModuleOriginY);
        TVector2 rot = coords.Rotate(-fModuleRotation * TMath::Pi() / 180.);

        return rot;
    }

    /// Converts the coordinates (xMod,yMod) in the readout module reference
    /// system to the readout plane reference system.
    TVector2 TransformToPhysicalCoordinates(Double_t xMod, Double_t yMod) {
        TVector2 coords(xMod, yMod);

        coords = coords.Rotate(fModuleRotation * TMath::Pi() / 180.);
        coords = coords + TVector2(fModuleOriginX, fModuleOriginY);

        return coords;
    }

   protected:
   public:
    // Setters

    ///  Sets the module by id definition
    void SetModuleID(Int_t modID) { fModuleID = modID; }

    /// Sets the module size by definition using (sX, sY) coordinates
    void SetSize(Double_t sX, Double_t sY) {
        fModuleSizeX = sX;
        fModuleSizeY = sY;
    }

    /// Sets the module size by definition using TVector2 input
    void SetSize(TVector2 s) {
        fModuleSizeX = s.X();
        fModuleSizeY = s.Y();
    }

    /// Sets the module origin by definition using (x,y) coordinates
    void SetOrigin(Double_t x, Double_t y) {
        fModuleOriginX = x;
        fModuleOriginY = y;
    }

    /// Sets the module origin by definition using TVector2 input
    void SetOrigin(TVector2 c) {
        fModuleOriginX = c.X();
        fModuleOriginY = c.Y();
    }

    /// Sets the module rotation in degrees
    void SetRotation(Double_t rot) { fModuleRotation = rot; }

    /// Sets the name of the readout module
    void SetName(TString name) { fModuleName = name; }

    /// Sets the tolerance for independent pixel overlaps
    void SetTolerance(Double_t tol) { fTolerance = tol; }

    /// Gets the tolerance for independent pixel overlaps
    inline Double_t GetTolerance() const { return fTolerance; }

    /// Returns the minimum daq id number
    inline Int_t GetMinDaqID() const { return fMininimumDaqId; }

    /// Returns the maximum daq id number
    inline Int_t GetMaxDaqID() const { return fMaximumDaqId; }

    /// Returns the physical readout channel index for a given daq id channel
    /// number
    Int_t DaqToReadoutChannel(Int_t daqChannel) {
        for (int n = 0; n < GetNumberOfChannels(); n++)
            if (GetChannel(n)->GetDaqID() == daqChannel) return n;
        return -1;
    }

    /// Returns the module id
    inline Int_t GetModuleID() const { return fModuleID; }

    /// Returns the module x-coordinate origin
    inline Double_t GetModuleOriginX() const { return fModuleOriginX; }

    /// Returns the module y-coordinate origin
    inline Double_t GetModuleOriginY() const { return fModuleOriginY; }

    /// Returns the module x-coordinate origin
    inline Double_t GetOriginX() const { return fModuleOriginX; }

    /// Returns the module y-coordinate origin
    inline Double_t GetOriginY() const { return fModuleOriginY; }

    /// Returns the module size x-coordinate
    inline Double_t GetModuleSizeX() const { return fModuleSizeX; }

    /// Returns the module size y-coordinate
    inline Double_t GetModuleSizeY() const { return fModuleSizeY; }

    /// Returns the module rotation in degrees
    inline Double_t GetModuleRotation() const { return fModuleRotation; }

    /// Converts the coordinates given by TVector2 in the readout plane reference
    /// system to the readout module reference system.
    TVector2 GetModuleCoordinates(TVector2 p) { return TransformToModuleCoordinates(p); }

    /// Converts the coordinates given by TVector2 in the readout module reference
    /// system to the readout plane reference system.
    TVector2 GetPhysicalCoordinates(TVector2 p) { return TransformToPhysicalCoordinates(p.X(), p.Y()); }

    /// Returns the module name
    inline char* GetName() const { return const_cast<char*>(fModuleName.Data()); }

    /// Returns a pointer to the readout mapping
    TRestDetectorReadoutMapping* GetMapping() { return &fMapping; }

    TRestDetectorReadoutChannel& operator[](int n) { return fReadoutChannel[n]; }

    /// Returns a pointer to a readout channel by index
    TRestDetectorReadoutChannel* GetChannel(int n) {
        if (n >= GetNumberOfChannels()) return nullptr;
        return &fReadoutChannel[n];
    }

    /// Returns the total number of channels defined inside the module
    Int_t GetNumberOfChannels() { return fReadoutChannel.size(); }

    /// Enables warning output
    void EnableWarnings() { showWarnings = true; }

    /// Disables warning output
    void DisableWarnings() { showWarnings = false; }

    void DoReadoutMapping(Int_t nodes = 0);

    Bool_t isInside(Double_t x, Double_t y);
    Bool_t isInside(TVector2 pos);

    Bool_t isInsideChannel(Int_t channel, Double_t x, Double_t y);
    Bool_t isInsideChannel(Int_t channel, TVector2 pos);

    Bool_t isInsidePixel(Int_t channel, Int_t pixel, Double_t x, Double_t y);
    Bool_t isInsidePixel(Int_t channel, Int_t pixel, TVector2 pos);

    Bool_t isDaqIDInside(Int_t daqID);
    Int_t FindChannel(Double_t x, Double_t y);
    TVector2 GetDistanceToModule(TVector2 pos);

    TVector2 GetPixelOrigin(Int_t channel, Int_t pixel);
    TVector2 GetPixelVertex(Int_t channel, Int_t pixel, Int_t vertex);
    TVector2 GetPixelCenter(Int_t channel, Int_t pixel);
    Bool_t GetPixelTriangle(Int_t channel, Int_t pixel);

    TVector2 GetPixelOrigin(TRestDetectorReadoutPixel* pix);
    TVector2 GetPixelVertex(TRestDetectorReadoutPixel* pix, Int_t vertex);
    TVector2 GetPixelCenter(TRestDetectorReadoutPixel* pix);
    Bool_t GetPixelTriangle(TRestDetectorReadoutPixel* pix);

    TVector2 GetVertex(int n) const;

    void AddChannel(TRestDetectorReadoutChannel& rChannel);

    void SetMinMaxDaqIDs();

    void Draw();

    void Print(Int_t DetailLevel = 0);

    // Construtor
    TRestDetectorReadoutModule();
    // Destructor
    virtual ~TRestDetectorReadoutModule();

    ClassDef(TRestDetectorReadoutModule, 1);
};
#endif
