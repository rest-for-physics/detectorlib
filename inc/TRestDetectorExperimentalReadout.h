/**
* @file TRestDetectorExperimentalReadout.h
* @brief Defines the TRestDetectorExperimentalReadout class.
*/

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUT_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUT_H

#include <TNamed.h>

#include <map>

#include "TRestDetectorExperimentalReadoutModule.h"

/**
* @class TRestDetectorExperimentalReadout
* @brief Represents an experimental readout detector.
* @inherits TNamed
* @author Luis Obis
*/
class TRestDetectorExperimentalReadout : public TNamed {
  private:
   std::vector<TRestDetectorExperimentalReadoutModule> fModules; /**< The modules of the detector. */

  public:
   /**
    * @brief Constructs a TRestDetectorExperimentalReadout object.
    */
   TRestDetectorExperimentalReadout() = default;

   /**
    * @brief Destructor for TRestDetectorExperimentalReadout.
    */
   ~TRestDetectorExperimentalReadout() override = default;

   /**
    * @brief Returns the number of modules in the detector.
    * @return The number of modules.
    */
   size_t GetNumberOfModules() { return fModules.size(); }

   /**
    * @brief Adds a module to the detector.
    * @param module The module to add.
    */
   void AddModule(const TRestDetectorExperimentalReadoutModule& module) { fModules.push_back(module); }

   /**
    * @brief Returns the modules of the detector.
    * @return The modules.
    */
   std::vector<const TRestDetectorExperimentalReadoutModule*> GetModules() const;

   /**
    * @brief Returns the modules that contain a given point.
    * @param point The point to check.
    * @return The modules containing the point.
    */
   std::vector<const TRestDetectorExperimentalReadoutModule*> GetModulesInPoint(const TVector3& point) const;

   ClassDef(TRestDetectorExperimentalReadout, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUT_H
