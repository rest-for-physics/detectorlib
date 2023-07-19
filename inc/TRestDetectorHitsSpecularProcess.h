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

#ifndef RestCore_TRestDetectorHitsSpecularProcess
#define RestCore_TRestDetectorHitsSpecularProcess

#include <TRestDetectorHitsEvent.h>
#include <TRestEventProcess.h>

/// A process to rotate hits from a given center and axis
class TRestDetectorHitsSpecularProcess : public TRestEventProcess {
   private:
    /// A pointer to the process input event
    TRestDetectorHitsEvent* fInputHitsEvent;  //!

    /// A pointer to the process output event
    TRestDetectorHitsEvent* fOutputHitsEvent;  //!

    void InitFromConfigFile() override;
    void Initialize() override;

   protected:
    /// Defines the position of the plane. A point inside the plane.
    TVector3 fPosition = {0, 0, 0};  //<

    /// Defines the normal to the plane we use to generate the specular image
    TVector3 fNormal = {0, 0, 1};  //<

   public:
    any GetInputEvent() const override { return fInputHitsEvent; }
    any GetOutputEvent() const override { return fOutputHitsEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    const char* GetProcessName() const override { return "specularProcess"; }

    inline TVector3 GetNormal() const { return fNormal; }
    inline TVector3 GetPosition() const { return fPosition; }

    TRestDetectorHitsSpecularProcess();
    TRestDetectorHitsSpecularProcess(const char* configFilename);
    ~TRestDetectorHitsSpecularProcess();

    ClassDefOverride(TRestDetectorHitsSpecularProcess, 1);
};
#endif
