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
                                           double rotationInDegrees = 0) {
        fPosition = position;
        fHeight = height;
        fNormal = normal;

        fCoordinateAxes = {{1, 0, 0}, {0, 1, 0}};

        // rotate the coordinate axes by rotationInDegrees around the normal vector
        fCoordinateAxes.first.Rotate(rotationInDegrees * TMath::RadToDeg(), fNormal);
        fCoordinateAxes.second.Rotate(rotationInDegrees * TMath::RadToDeg(), fNormal);
    }

    virtual ~TRestDetectorExperimentalReadoutModule() = default;
    TRestDetectorExperimentalReadoutModule() = default;

    size_t GetNumberOfPixels() const { return fPixels.size(); }

    std::vector<TRestDetectorExperimentalReadoutPixel> GetPixels() const { return fPixels; }

    void AddPixel(const TRestDetectorExperimentalReadoutPixel& pixel, id pixelId = 0) {
        fPixels.push_back(pixel);
    }

    std::vector<TVector2> GetConvexHull() const {
        std::vector<TVector2> vertices;

        for (auto& pixel : fPixels) {
            for (auto& vertex : pixel.GetVertices()) {
                vertices.push_back(vertex);
            }
        }

        return TRestDetectorExperimentalReadoutPixel::findConvexHull(vertices);
    }

    ClassDef(TRestDetectorExperimentalReadoutModule, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
