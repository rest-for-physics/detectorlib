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

#ifndef RestCore_TRestDetectorVetoReadout
#define RestCore_TRestDetectorVetoReadout

#include <TRestMetadata.h>

#include <iostream>

#include "TRestDetectorReadoutPlane.h"

class TRestDetectorVetoReadout : public TRestMetadata {
   private:
    std::string fGeometry;

    std::vector<TRestDetectorReadoutPlane> fReadoutPlanes;

    std::vector<std::string> fVetoVolumeNames;
    std::vector<std::string> fVetoBoundaryVolumeNames;  /// aux volume used to identify the readout plane
    std::map<std::string, int> fVetoVolumeNamesToSignalIdMap;

   public:
    TRestDetectorVetoReadout();
    TRestDetectorVetoReadout(const char* configFilename, const char* name);

    void PrintMetadata() override;

    ClassDefOverride(TRestDetectorVetoReadout, 1);
};

#endif
