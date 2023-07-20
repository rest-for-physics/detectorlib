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

#ifndef RestCore_TRestDetectorHitmapTransformationProcess
#define RestCore_TRestDetectorHitmapTransformationProcess

#include <TRestDetectorHitsEvent.h>

#include "TRestEventProcess.h"

/// A structure to define a given hit transformation
struct HitTransformation {
    /// The center of rotation or a point contained in the specular plane
    TVector3 position = {0, 0, 0};

    /// The translation vector, the axis of rotation or the specular plane normal
    TVector3 vector = {0, 0, 1};

    /// The angle of rotation
    Double_t angle = 0;

    /// The type of transformation (specular/rotation/translation)
    std::string type = "";

    /// A given name that allows to place an order to the transformations
    std::string name = "";
};

//! An analysis process to apply rotation/translation/specular to mean hit positions
class TRestDetectorHitmapTransformationProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestDetectorHitsEvent input
    TRestDetectorHitsEvent* fHitsEvent;  //!

    void InitFromConfigFile() override;

    void Initialize() override;

   protected:
    /// A list of transformations that can be applied to the mean positions
    std::vector<HitTransformation> fTransDefinitions;

    /// An ordered list with the names of transformations that will be applied
    std::vector<std::string> fTransformations;

    /// It returns the transformation with a given name
    HitTransformation GetTransformation(const std::string& name);

    TVector3 Specular(const TVector3& pos, const HitTransformation& tr);
    TVector3 Rotation(const TVector3& pos, const HitTransformation& tr);
    TVector3 Translation(const TVector3& pos, const HitTransformation& tr);

   public:
    any GetInputEvent() const override { return fHitsEvent; }
    any GetOutputEvent() const override { return fHitsEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    const char* GetProcessName() const override { return "hitsTransformation"; }

    TRestDetectorHitmapTransformationProcess();
    TRestDetectorHitmapTransformationProcess(const char* configFilename);
    ~TRestDetectorHitmapTransformationProcess();

    ClassDefOverride(TRestDetectorHitmapTransformationProcess, 1);
};
#endif
