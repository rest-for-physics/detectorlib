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
#include <TVector2.h>

#include <iostream>

#include "TRestDetectorReadoutChannel.h"
#include "TRestDetectorReadoutMapping.h"

/// A class to store the readout module definition used in TRestDetectorReadoutPlane. It
/// allows to integrate any number of independent readout channels.
class TRestDetectorReadoutModule {
   private:
    Int_t fModuleID;  ///< The module id given by the readout definition.

    TString fModuleName;  ///< The assigned module name.

    TVector2 fModuleOrigin;  ///< The module (x, y) position relative to the readout plane position.

    TVector2 fModuleSize;  ///< The module (x, y) size. All pixels should be contained within this size.

    Double_t fModuleRotation;  ///< The rotation of the module around the
                               ///< position=(fModuleOriginX, fModuleOriginY) in
                               ///< degrees.

    Int_t fMinimumDaqId;  ///< The minimum daq channel id associated to the
                          ///< module.
    Int_t fMaximumDaqId;  ///< The maximum daq channel id associated to the module.

    std::vector<TRestDetectorReadoutChannel>
        fReadoutChannel;  ///< A std::vector of the instances of TRestDetectorReadoutChannel
                          ///< contained in the readout module.

    TRestDetectorReadoutMapping fMapping;  ///< The readout module uniform grid mapping.

    Double_t fTolerance;  ///< Tolerance allowed in overlaps at the pixel
                          ///< boundaries in mm.

    Bool_t showWarnings;  //!///< Flag to enable/disable warning outputs. Disabled by
                          //! default. REST_Warning in TRestDetectorReadout will enable it.

    void Initialize();

    /// Converts the coordinates (xPhys,yPhys) in the readout plane reference
    /// system to the readout module reference system.
    inline TVector2 TransformToModuleCoordinates(const TVector2& xyPhysical) const {
        auto coords = xyPhysical - fModuleOrigin;
        TVector2 rot = coords.Rotate(-fModuleRotation * TMath::Pi() / 180.);

        return rot;
    }

    /// Converts the coordinates (xMod,yMod) in the readout module reference
    /// system to the readout plane reference system.
    inline TVector2 TransformToPhysicalCoordinates(Double_t xMod, Double_t yMod) const {
        TVector2 coords(xMod, yMod);

        coords = coords.Rotate(fModuleRotation * TMath::Pi() / 180.);
        coords += fModuleOrigin;

        return coords;
    }

   protected:
   public:
    // Setters

    ///  Sets the module by id definition
    inline void SetModuleID(Int_t modID) { fModuleID = modID; }

    /// Sets the module size by definition using TVector2 input
    inline void SetSize(const TVector2& size) { fModuleSize = size; }

    /// Sets the module origin by definition using TVector2 input
    inline void SetOrigin(const TVector2& origin) { fModuleOrigin = origin; }

    /// Sets the module origin by definition using (x,y) coordinates
    inline void SetOrigin(Double_t x, Double_t y) { SetOrigin({x, y}); }

    /// Sets the module rotation in degrees
    inline void SetRotation(Double_t rotation) { fModuleRotation = rotation; }

    /// Sets the name of the readout module
    inline void SetName(const TString& name) { fModuleName = name; }

    /// Sets the tolerance for independent pixel overlaps
    inline void SetTolerance(Double_t tolerance) { fTolerance = tolerance; }

    /// Gets the tolerance for independent pixel overlaps
    inline Double_t GetTolerance() const { return fTolerance; }

    /// Returns the minimum daq id number
    inline Int_t GetMinDaqID() const { return fMinimumDaqId; }

    /// Returns the maximum daq id number
    inline Int_t GetMaxDaqID() const { return fMaximumDaqId; }

    /// Returns the physical readout channel index for a given daq id channel
    /// number
    inline Int_t DaqToReadoutChannel(Int_t daqChannel) const {
        for (auto n = 0; n < GetNumberOfChannels(); n++)
            if (GetChannel(n)->GetDaqID() == daqChannel) {
                return n;
            }
        return -1;
    }

    /// Returns the module id
    inline Int_t GetModuleID() const { return fModuleID; }

    /// Returns the module x-coordinate origin
    inline Double_t GetModuleOriginX() const { return fModuleOrigin.X(); }

    /// Returns the module y-coordinate origin
    inline Double_t GetModuleOriginY() const { return fModuleOrigin.Y(); }

    /// Returns the module x-coordinate origin
    inline Double_t GetOriginX() const { return fModuleOrigin.X(); }

    /// Returns the module y-coordinate origin
    inline Double_t GetOriginY() const { return fModuleOrigin.Y(); }

    /// Returns the module size x-coordinate
    inline Double_t GetModuleSizeX() const { return fModuleSize.X(); }

    /// Returns the module size y-coordinate
    inline Double_t GetModuleSizeY() const { return fModuleSize.Y(); }

    /// Returns the module rotation in degrees
    inline Double_t GetModuleRotation() const { return fModuleRotation; }

    /// Converts the coordinates given by TVector2 in the readout plane reference
    /// system to the readout module reference system.
    TVector2 GetModuleCoordinates(const TVector2& p) const { return TransformToModuleCoordinates(p); }

    /// Converts the coordinates given by TVector2 in the readout module reference
    /// system to the readout plane reference system.
    TVector2 GetPhysicalCoordinates(const TVector2& p) const {
        return TransformToPhysicalCoordinates(p.X(), p.Y());
    }

    /// Returns the module name
    inline const char* GetName() const { return fModuleName.Data(); }

    /// Returns a pointer to the readout mapping
    inline const TRestDetectorReadoutMapping* GetMapping() const { return &fMapping; }

    inline TRestDetectorReadoutChannel& operator[](int n) { return fReadoutChannel[n]; }

    /// Returns a const pointer to a readout channel by index
    inline const TRestDetectorReadoutChannel* GetChannel(int n) const {
        if (n >= GetNumberOfChannels()) {
            return nullptr;
        }
        return &fReadoutChannel[n];
    }

    /// Returns a pointer to a readout channel by index
    inline TRestDetectorReadoutChannel* GetChannel(int n) {
        if (n >= GetNumberOfChannels()) {
            return nullptr;
        }
        return &fReadoutChannel[n];
    }

    /// Returns the total number of channels defined inside the module
    inline size_t GetNumberOfChannels() const { return fReadoutChannel.size(); }

    /// Enables warning output
    inline void EnableWarnings() { showWarnings = true; }

    /// Disables warning output
    inline void DisableWarnings() { showWarnings = false; }

    void DoReadoutMapping(Int_t nodes = 0);

    Bool_t isInside(const TVector2& position) const;

    ///////////////////////////////////////////////
    /// \brief Determines if the position *x,y* relative to the readout
    /// plane are inside this readout module.
    ///
    inline Bool_t isInside(Double_t x, Double_t y) const { return isInside({x, y}); }

    Bool_t isInsideChannel(Int_t channel, Double_t x, Double_t y) const;
    Bool_t isInsideChannel(Int_t channel, const TVector2& position) const;

    Bool_t isInsidePixel(Int_t channel, Int_t pixel, const TVector2& position) const;

    Bool_t isDaqIDInside(Int_t daqID) const;
    Int_t FindChannel(const TVector2& position) const;
    TVector2 GetDistanceToModule(const TVector2& position) const;

    TVector2 GetPixelOrigin(Int_t channel, Int_t pixel) const;
    TVector2 GetPixelVertex(Int_t channel, Int_t pixel, Int_t vertex) const;
    TVector2 GetPixelCenter(Int_t channel, Int_t pixel) const;
    Bool_t GetPixelTriangle(Int_t channel, Int_t pixel) const;

    TVector2 GetPixelOrigin(TRestDetectorReadoutPixel* pix) const;
    TVector2 GetPixelVertex(TRestDetectorReadoutPixel* pix, Int_t vertex) const;
    TVector2 GetPixelCenter(TRestDetectorReadoutPixel* pix) const;
    Bool_t GetPixelTriangle(TRestDetectorReadoutPixel* pix) const;

    TVector2 GetVertex(int n) const;

    void AddChannel(TRestDetectorReadoutChannel& rChannel);

    void SetMinMaxDaqIDs();

    void Draw();

    void Print(Int_t DetailLevel = 0);

    // Constructor
    TRestDetectorReadoutModule();
    // Destructor
    virtual ~TRestDetectorReadoutModule();

    ClassDef(TRestDetectorReadoutModule, 2);
};
#endif
