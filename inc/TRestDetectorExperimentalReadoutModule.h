//
// Created by lobis on 01-Jun-23.
//

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H

#include <map>

#include "TRestDetectorExperimentalReadoutPixel.h"

class TRestDetectorExperimentalReadoutModule {
   private:
    std::vector<TRestDetectorExperimentalReadoutPixel> fPixels;

    TVector3 fPosition;  // position of the module in 3D space
    TVector3 fNormal;    // normal vector to the module
    double fHeight;      // height of the module (drift distance)
    std::pair<TVector3, TVector3> fCoordinateAxes;

   public:
    TRestDetectorExperimentalReadoutModule(const TVector3& position, double height, const TVector3& normal,
                                           double rotationInDegrees = 0);

    // needed for root
    TRestDetectorExperimentalReadoutModule() = default;
    virtual ~TRestDetectorExperimentalReadoutModule() = default;

    size_t GetNumberOfPixels() const { return fPixels.size(); }

    std::vector<TRestDetectorExperimentalReadoutPixel> GetPixels() const { return fPixels; }

    void InsertPixel(const TRestDetectorExperimentalReadoutPixel& pixel);

    std::vector<TVector2> GetConvexHull() const;

    ClassDef(TRestDetectorExperimentalReadoutModule, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
