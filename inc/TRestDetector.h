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

#ifndef RestCore_TRestDetector
#define RestCore_TRestDetector

#include <Rtypes.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TRestMetadata.h"
#include "TRestStringOutput.h"
#include "TRestTools.h"



//! An metadata class to store basic detector information
class TRestDetector : public TRestMetadata {
   public:
    /// The detector name
    std::string fDetectorName = "REST Detector";

    /// The detector drift voltage in V
    Double_t fDriftVoltage = -1;
    /// The detector drift voltage in V/mm
    Double_t fDriftField = -1;
    /// The detector drift distance in mm
    Double_t fDriftDistance = -1;
    /// The detector amplification voltage in V
    Double_t fAmplificationVoltage = -1;
    /// The detector amplification distance in mm (also known as amplificationGap)
    Double_t fAmplificationDistance = -1;
    /// The detector amplification gain
    Double_t fAmplificationGain = -1;

    /// The detector active mass in kg
    Double_t fMass = -1;
    /// The detector active volume height in mm
    Double_t fHeight = -1;
    /// The detector active volume radius in mm
    Double_t fRadius = -1;
    /// The detector active volume length in mm
    Double_t fLength = -1;
    /// The detector active volume width in mm
    Double_t fWidth = -1;
    /// The detector active volume pressure in atm
    Double_t fPressure = -1;

    /// The electronics shaping time in us
    Double_t fElectronicsShapingTime = -1;
    /// The electronics sampling time in us
    Double_t fElectronicsSamplingTime = -1;
    /// The electronics dynamic range in V
    Double_t fElectronicsDynamicRange = -1;
    /// The electronics threshold in ADC units
    Double_t fElectronicsThreshold = -1;

    /// The electronics gain in raw configuration format (hexadecimal)
    std::string fElectronicsGain = "-1";
    /// The electronics clock (sampling) in raw configuration format (hexadecimal)
    std::string fElectronicsClock = "-1";
    /// The electronics shaping in raw configuration format (hexadecimal)
    std::string fElectronicsShaping = "-1";

    void InitFromConfigFile() { ReadAllParameters(); }

    void PrintMetadata();

    void UpdateMetadataMembers();

    // Constructors
    TRestDetector() {}
    // Destructor
    ~TRestDetector() {}

    ClassDef(TRestDetector, 4);
};

#endif
