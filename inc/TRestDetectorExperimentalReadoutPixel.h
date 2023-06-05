//
// Created by lobis on 01-Jun-23.
//

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H

#include <TVector2.h>
#include <TVector3.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

typedef unsigned short id;

class TRestDetectorExperimentalReadoutPixel {
   private:
    std::vector<TVector2> fVertices;
    TVector2 fCenter;
    double fRadius;

    void InitializeVertices(const std::vector<TVector2>& vertices);

    static std::vector<TVector2> GetRectangularVertices(const TVector2& center,
                                                        const std::pair<double, double>& size);

   public:
    explicit TRestDetectorExperimentalReadoutPixel(const std::vector<TVector2>& vertices);

    TRestDetectorExperimentalReadoutPixel(const TVector2& center, const std::pair<double, double>& size);

    TRestDetectorExperimentalReadoutPixel(const TVector2& center, double size);

    // needed for root
    TRestDetectorExperimentalReadoutPixel() = default;
    virtual ~TRestDetectorExperimentalReadoutPixel() = default;

    TVector2 GetCenter() const { return fCenter; }
    double GetRadius() const { return fRadius; }

    std::vector<TVector2> GetVertices() const { return fVertices; }

    bool IsInside(const TVector2& point) const;

    ClassDef(TRestDetectorExperimentalReadoutPixel, 1);
};

namespace readout {

// Helper function to calculate the cross product of two vectors
double crossProduct(const TVector2& A, const TVector2& B, const TVector2& C);

// Helper function to calculate the squared distance between two points
double squaredDistance(const TVector2& A, const TVector2& B);

// Comparator function to compare two points based on their polar angle with respect to the anchor point
bool comparePoints(const TVector2& A, const TVector2& B, const TVector2& anchor);
// Function to find the convex hull of a set of points using the Graham's scan algorithm
std::vector<TVector2> findConvexHull(const std::vector<TVector2>& _points);

bool IsPointInsideConvexHull(const TVector2& point, const std::vector<TVector2>& convexHull);

}  // namespace readout
#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H
