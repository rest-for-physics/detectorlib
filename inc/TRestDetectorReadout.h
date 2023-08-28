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

#ifndef RestCore_TRestDetectorReadout
#define RestCore_TRestDetectorReadout

#include <TRestMetadata.h>

#include <iostream>

#include "TRestDetectorReadoutPlane.h"

/// A metadata class to generate/store a readout description.
class TRestDetectorReadout : public TRestMetadata {
   private:
    void InitFromConfigFile() override;

    void Initialize() override;

    std::vector<TRestDetectorReadoutPlane>
        fReadoutPlanes;  ///< A std::vector storing the TRestDetectorReadoutPlane definitions.

    Int_t fMappingNodes;  //!///< Number of nodes per axis used on the readout
                          //! coordinate mapping. See also TRestDetectorReadoutMapping.
    std::vector<TRestDetectorReadoutModule> fModuleDefinitions;  //!///< A std::vector storing the different
                                                                 //! TRestDetectorReadoutModule definitions.

    void ValidateReadout() const;

   public:
    TRestDetectorReadoutPlane& operator[](int p) { return fReadoutPlanes[p]; }

    TRestDetectorReadoutPlane* GetReadoutPlane(int p);
    void AddReadoutPlane(const TRestDetectorReadoutPlane& plane);

    /////////////////////////////////////
    TRestDetectorReadoutPlane* GetReadoutPlaneWithID(int id);
    TRestDetectorReadoutModule* GetReadoutModuleWithID(int id);
    TRestDetectorReadoutChannel* GetReadoutChannelWithDaqID(int daqId);
    /////////////////////////////////////

    Int_t GetNumberOfReadoutPlanes() const { return fReadoutPlanes.size(); }
    Int_t GetNumberOfModules();
    Int_t GetNumberOfChannels();

    Int_t GetModuleDefinitionId(const TString& name);

    /////////////////////////////////////
    TRestDetectorReadoutModule* ParseModuleDefinition(TiXmlElement* moduleDefinition);
    void GetPlaneModuleChannel(Int_t daqID, Int_t& planeID, Int_t& moduleID, Int_t& channelID);
    Int_t GetHitsDaqChannel(const TVector3& position, Int_t& planeID, Int_t& moduleID, Int_t& channelID);

    /// Returns a tuple with the DaqID, ModuleID, ChannelID
    std::tuple<Int_t, Int_t, Int_t> GetHitsDaqChannelAtReadoutPlane(const TVector3& position,
                                                                    Int_t planeId = 0);

    /// \brief Returns the DaqID of the channel for position. If no channel is found returns -1
    Int_t GetDaqId(const TVector3& position, bool check = false);

    Double_t GetX(Int_t signalID);
    Double_t GetY(Int_t signalID);
    /////////////////////////////////////

    Double_t GetX(Int_t planeID, Int_t modID, Int_t chID);
    Double_t GetY(Int_t planeID, Int_t modID, Int_t chID);

    // Detail Level:
    // 0->this readout
    // 1->+all readout plane
    // 2->+all readout module
    // 3->+all readout channel
    // 4->+all readout pixel
    inline void PrintMetadata() override { PrintMetadata(1); }
    void PrintMetadata(Int_t DetailLevel);

    void Draw();

    void Export(const std::string& fileName);

    // Constructor
    TRestDetectorReadout();
    explicit TRestDetectorReadout(const char* configFilename);
    TRestDetectorReadout(const char* configFilename, const std::string& name);
    // Destructor
    ~TRestDetectorReadout() override;

    ClassDefOverride(TRestDetectorReadout, 3);
};
#endif
