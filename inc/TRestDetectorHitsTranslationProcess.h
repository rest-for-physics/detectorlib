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

#ifndef RestCore_TRestDetectorHitsTranslationProcess
#define RestCore_TRestDetectorHitsTranslationProcess

#include <TRestDetectorHitsEvent.h>
#include <TRestEventProcess.h>

/// A process to translate hits by a given user amount
class TRestDetectorHitsTranslationProcess : public TRestEventProcess {
   private:
    /// A pointer to the process input event
    TRestDetectorHitsEvent* fInputEvent;  //!

    /// A pointer to the process output event
    TRestDetectorHitsEvent* fOutputEvent;  //!

    void InitFromConfigFile() override;
    void Initialize() override;

   protected:
    /// The amount to be translated every hit
    TVector3 fTranslation = {0, 0, 0};  //<

   public:
    RESTValue GetInputEvent() const override { return fInputEvent; }
    RESTValue GetOutputEvent() const override { return fOutputEvent; }

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void PrintMetadata() override;

    const char* GetProcessName() const override { return "hitsTranslation"; }

    inline TVector3 GetTranslation() const { return fTranslation; }

    TRestDetectorHitsTranslationProcess();
    TRestDetectorHitsTranslationProcess(const char* configFilename);
    ~TRestDetectorHitsTranslationProcess();

    ClassDefOverride(TRestDetectorHitsTranslationProcess, 1);
};
#endif
