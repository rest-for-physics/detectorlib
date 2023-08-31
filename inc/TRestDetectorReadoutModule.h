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
    Int_t fId = -1;  ///< The module id given by the readout definition.

    TVector2 fOrigin = {0, 0};  ///< The module (x, y) position relative to the readout plane position.

    TVector2 fSize = {0, 0};  ///< The module (x, y) size. All pixels should be contained within this size.

    /// The rotation of the module around the module origin (fModuleOriginX, fModuleOriginY) in radians.
    Double_t fRotation = 0;  //<

    std::pair<Int_t, Int_t> fDaqIdRange = {
        -1, -1};  ///< The minimum and maximum daq channel ids associated to the module.

    std::vector<TRestDetectorReadoutChannel>
        fReadoutChannel;  ///< A std::vector of the instances of TRestDetectorReadoutChannel
                          ///< contained in the readout module.

    TRestDetectorReadoutMapping fMapping;  ///< The readout module uniform grid mapping.

    Double_t fTolerance;  ///< Tolerance allowed in overlaps at the pixel
                          ///< boundaries in mm.

    Bool_t showWarnings;  //!///< Flag to enable/disable warning outputs. Disabled by
                          //! default. REST_Warning in TRestDetectorReadout will enable it.

    Int_t fFirstDaqChannel = 0;  ///< First DAQ channel

    std::string fDecodingFile = "";  ///< Decoding file

    Int_t fMappingNodes = 0;  ///< Number of nodes

    Bool_t fDecoding;  ///< Defines if a decoding file was used to set the relation
                       ///< between a physical readout channel id and a signal daq id

    std::string fName;  //<
    std::string fType;  //<

    void Initialize();

    /// Converts the coordinates (xPhys,yPhys) in the readout plane reference
    /// system to the readout module reference system.
    inline TVector2 TransformToModuleCoordinates(const TVector2& coords) const {
        return TVector2(coords - fOrigin).Rotate(-1.0 * fRotation);
    }

    /// Converts the coordinates (xMod,yMod) in the readout module reference
    /// system to the readout plane reference system.
    inline TVector2 TransformToPlaneCoordinates(const TVector2& coords) const {
        return coords.Rotate(fRotation) + fOrigin;
    }

   protected:
   public:
    // Setters

    ///  Sets the module by id definition
    inline void SetModuleID(Int_t modID) { fId = modID; }

    /// Sets the module size by definition using TVector2 input
    inline void SetSize(const TVector2& size) { fSize = size; }

    /// Sets the module origin by definition using TVector2 input
    inline void SetOrigin(const TVector2& origin) { fOrigin = origin; }

    /// Sets the module rotation in degrees
    inline void SetRotation(Double_t rotation) { fRotation = rotation; }

    /// Sets the name of the readout module
    inline void SetName(const std::string& name) { fName = name; }

    /// Sets the type of the readout module
    inline void SetType(const std::string& type) { fType = type; }

    /// Sets the tolerance for independent pixel overlaps
    inline void SetTolerance(Double_t tolerance) { fTolerance = tolerance; }

    /// Sets first DAQ channel
    inline void SetFirstDaqChannel(Int_t firstDaqChannel) { fFirstDaqChannel = firstDaqChannel; }

    /// Sets number of nodes
    inline void SetMappingNodes(Int_t nodes) { fMappingNodes = nodes; }

    /// Gets the tolerance for independent pixel overlaps
    inline Double_t GetTolerance() const { return fTolerance; }

    /// Returns the minimum daq id number
    inline Int_t GetMinDaqID() const { return fDaqIdRange.first; }

    /// Returns the maximum daq id number
    inline Int_t GetMaxDaqID() const { return fDaqIdRange.second; }

    inline Int_t GetMappingNodes() const { return fMappingNodes; }

    /// Returns the physical readout channel index for a given daq id channel number
    inline Int_t DaqToReadoutChannel(Int_t daqChannel) {
        for (size_t n = 0; n < GetNumberOfChannels(); n++) {
            if (GetChannel(n)->GetDaqID() == daqChannel) {
                return n;
            }
        }
        return -1;
    }

    /// Returns the module id
    inline Int_t GetModuleID() const { return fId; }

    /// Returns the module origin position
    inline TVector2 GetOrigin() const { return fOrigin; }

    /// Returns the module size (x, y) in mm
    inline TVector2 GetSize() const { return fSize; }

    /// Returns the module rotation in degrees
    inline Double_t GetRotation() const { return fRotation; }

    /// Converts the coordinates given by TVector2 in the readout plane reference
    /// system to the readout module reference system.
    TVector2 GetModuleCoordinates(const TVector2& p) const { return TransformToModuleCoordinates(p); }

    /// Converts the coordinates given by TVector2 in the readout module reference
    /// system to the readout plane reference system.
    TVector2 GetPlaneCoordinates(const TVector2& p) { return TransformToPlaneCoordinates(p); }

    /// Returns the module name
    inline std::string GetName() const { return fName; }
    inline std::string GetType() const { return fType; }

    /// Returns a pointer to the readout mapping
    inline TRestDetectorReadoutMapping* GetMapping() { return &fMapping; }

    inline TRestDetectorReadoutChannel& operator[](int n) { return fReadoutChannel[n]; }

    /// Returns a pointer to a readout channel by index
    inline TRestDetectorReadoutChannel* GetChannel(size_t n) {
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

    void DoReadoutMapping();

    void SetDecodingFile(const std::string& decodingFile);

    ///////////////////////////////////////////////
    /// \brief Determines if the position *x,y* relative to the readout
    /// plane are inside this readout module.
    ///
    Bool_t IsInside(const TVector2& position) const;

    Bool_t IsInsideChannel(Int_t channel, const TVector2& position);

    Bool_t IsInsidePixel(Int_t channel, Int_t pixel, const TVector2& position);

    Bool_t IsDaqIDInside(Int_t daqID);
    Int_t FindChannel(const TVector2& position);
    TVector2 GetDistanceToModule(const TVector2& position);

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

    // Constructor
    TRestDetectorReadoutModule();
    // Destructor
    virtual ~TRestDetectorReadoutModule();

    ClassDef(TRestDetectorReadoutModule, 5);
};
#endif
