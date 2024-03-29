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

#ifndef RestCore_TRestDetectorReadoutChannel
#define RestCore_TRestDetectorReadoutChannel

#include <TRestMetadata.h>

#include <iostream>

#include "TRestDetectorReadoutPixel.h"

/// A class to store the readout channel definition used in TRestDetectorReadoutModule.
/// It allows to integrate any number of independent readout pixels.
class TRestDetectorReadoutChannel {
   private:
    Int_t fDaqID;  ///< Defines the corresponding daq channel id. See decoding
                   ///< details at TRestDetectorReadout.
    std::vector<TRestDetectorReadoutPixel> fReadoutPixel;  ///< A std::vector storing the different
                                                           ///< TRestDetectorReadoutPixel definitions.

    Short_t fChannelId = -1;  ///< It stores the corresponding physical readout channel

    std::string fName;  // Name of the signal
    std::string fType;  // Type of the signal

    void Initialize();

   public:
    /// Returns the corresponding daq channel id
    inline Int_t GetDaqID() const { return fDaqID; }

    /// Returns the channel name
    inline std::string GetName() const { return fName; }

    /// Returns the channel type
    inline std::string GetType() const { return fType; }

    /// Returns the corresponding channel id
    inline Int_t GetChannelId() const { return fChannelId; }

    std::string GetChannelName() const { return fName; }
    std::string GetChannelType() const { return fType; }

    void SetChannelName(const std::string& name) { fName = name; }
    void SetChannelType(const std::string& type) { fType = type; }

    /// Returns the total number of pixels inside the readout channel
    Int_t GetNumberOfPixels() { return fReadoutPixel.size(); }

    TRestDetectorReadoutPixel& operator[](int n) { return fReadoutPixel[n]; }

    /// Returns a pointer to the pixel *n* by index.
    TRestDetectorReadoutPixel* GetPixel(int n) {
        if (n >= GetNumberOfPixels()) return nullptr;
        return &fReadoutPixel[n];
    }

    /// Sets the daq channel number id
    void SetDaqID(Int_t id) { fDaqID = id; }

    /// Sets the readout channel number id
    void SetChannelID(Int_t id) { fChannelId = id; }

    /// Adds a new pixel to the readout channel
    void AddPixel(const TRestDetectorReadoutPixel& pixel) { fReadoutPixel.emplace_back(pixel); }

    /// Sets the channel name
    void SetName(const std::string& name) { fName = name; }

    /// Sets the channel type
    void SetType(const std::string& type) { fType = type; }

    Int_t isInside(Double_t x, Double_t y);

    void Print(int DetailLevel = 0);

    // Constructor
    TRestDetectorReadoutChannel();
    // Destructor
    virtual ~TRestDetectorReadoutChannel();

    ClassDef(TRestDetectorReadoutChannel, 6);  // REST run class
};
#endif
