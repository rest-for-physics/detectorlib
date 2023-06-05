//
// Created by lobis on 01-Jun-23.
//

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H

#include <map>
#include <memory>

#include "TRestDetectorExperimentalReadoutPixel.h"

class KDTree;

class TRestDetectorExperimentalReadoutModule {
   private:
    std::vector<TRestDetectorExperimentalReadoutPixel> fPixels;

    TVector3 fPosition;  // position of the module in 3D space
    TVector3 fNormal;    // normal vector to the module
    double fHeight;      // height of the module (drift distance)
    std::pair<TVector3, TVector3> fCoordinateAxes;

    // auxiliary data structures
    // std::unique_ptr<MyKDTree> fKDTree;
    KDTree* fKDTree = nullptr;
    double fSearchRadius = 0;
    std::vector<TVector2> fConvexHull;

    void BuildKDTree();
    std::vector<TVector2> ComputeConvexHull();

   public:
    TRestDetectorExperimentalReadoutModule(const TVector3& position, double height, const TVector3& normal,
                                           double rotationInDegrees = 0);

    // needed for root
    TRestDetectorExperimentalReadoutModule() = default;
    virtual ~TRestDetectorExperimentalReadoutModule() {
        // delete fKDTree;
    }

    size_t GetNumberOfPixels() const { return fPixels.size(); }

    std::vector<TRestDetectorExperimentalReadoutPixel> GetPixels() const { return fPixels; }

    void SetPixels(const std::vector<TRestDetectorExperimentalReadoutPixel>&);

    std::vector<TVector2> GetConvexHull() const { return fConvexHull; }

    std::vector<TRestDetectorExperimentalReadoutPixel> GetPixelsInPoint(const TVector2& point) const;

    double GetZ(const TVector3& point) const;

    bool IsInside(const TVector2& point) const;
    bool IsInside(const TVector3& point) const;

    TVector2 TransformToModuleCoordinates(const TVector3& point) const;
    TVector3 TransformToAbsoluteCoordinates(const TVector2& point) const;

    ClassDef(TRestDetectorExperimentalReadoutModule, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
