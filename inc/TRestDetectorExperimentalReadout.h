//
// Created by lobis on 01-Jun-23.
//

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUT_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUT_H

#include <TNamed.h>

#include <map>

#include "TRestDetectorExperimentalReadoutModule.h"

class TRestDetectorExperimentalReadout : public TNamed {
   private:
    std::map<id, TRestDetectorExperimentalReadoutModule> fModules;
    // std::vector<TRestDetectorExperimentalReadoutModule> fModules;

   public:
    TRestDetectorExperimentalReadout() = default;
    ~TRestDetectorExperimentalReadout() override = default;

    // size_t GetNumberOfModules() { return fModules.size(); }

    ClassDef(TRestDetectorExperimentalReadout, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUT_H
