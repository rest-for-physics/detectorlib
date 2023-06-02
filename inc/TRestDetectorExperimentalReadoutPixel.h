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

    void InitializeVertices(const std::vector<TVector2>& vertices) {
        // at least three non-aligned vertices
        if (vertices.size() < 3) {
            throw std::invalid_argument("Pixel must have at least three vertices");
        }
        // compute convex hull
        std::vector<TVector2> convexHull = findConvexHull(vertices);
        fVertices = convexHull;
    }

    static std::vector<TVector2> GetRectangularVertices(const TVector2& center,
                                                        const std::pair<double, double>& size) {
        // rectangular pixel
        std::vector<TVector2> vertices = {{center.X() - size.first / 2, center.Y() - size.second / 2},
                                          {center.X() + size.first / 2, center.Y() - size.second / 2},
                                          {center.X() + size.first / 2, center.Y() + size.second / 2},
                                          {center.X() - size.first / 2, center.Y() + size.second / 2}};

        return vertices;
    }

    // Helper function to calculate the cross product of two vectors
    static double crossProduct(const TVector2& A, const TVector2& B, const TVector2& C) {
        return (B.X() - A.X()) * (C.Y() - A.Y()) - (B.Y() - A.Y()) * (C.X() - A.X());
    }

    // Helper function to calculate the squared distance between two points
    static double squaredDistance(const TVector2& A, const TVector2& B) {
        double dx = B.X() - A.X();
        double dy = B.Y() - A.Y();
        return dx * dx + dy * dy;
    }

    // Comparator function to compare two points based on their polar angle with respect to the anchor point
    static bool comparePoints(const TVector2& A, const TVector2& B, const TVector2& anchor) {
        double cross = crossProduct(anchor, A, B);
        if (cross == 0) {
            // If two points have the same polar angle, choose the one that is closer to the anchor point
            return squaredDistance(anchor, A) < squaredDistance(anchor, B);
        }
        return cross > 0;
    }

   public:
    explicit TRestDetectorExperimentalReadoutPixel(const std::vector<TVector2>& vertices) {
        InitializeVertices(vertices);
    }

    TRestDetectorExperimentalReadoutPixel(const TVector2& center, const std::pair<double, double>& size) {
        // rectangular pixel
        InitializeVertices(GetRectangularVertices(center, size));
    }

    TRestDetectorExperimentalReadoutPixel(const TVector2& center, double size) {
        // square pixel
        InitializeVertices(GetRectangularVertices(center, {size, size}));
    }

    TRestDetectorExperimentalReadoutPixel() = default;
    virtual ~TRestDetectorExperimentalReadoutPixel() = default;

    TVector2 GetCenter() const {
        TVector2 center(0, 0);
        for (auto& vertex : fVertices) {
            center += vertex;
        }
        center *= 1. / fVertices.size();
        return center;
    }

    std::vector<TVector2> GetVertices() const { return fVertices; }

    // Function to find the convex hull of a set of points using the Graham's scan algorithm
    static std::vector<TVector2> findConvexHull(const std::vector<TVector2>& _points) {
        std::vector<TVector2> points = _points;
        // Find the bottommost point as the anchor point
        TVector2 anchor = points[0];
        for (const TVector2& point : points) {
            if (point.Y() < anchor.Y() || (point.Y() == anchor.Y() && point.X() < anchor.X())) {
                anchor = point;
            }
        }

        // Sort the points based on their polar angle with respect to the anchor point
        std::sort(points.begin(), points.end(),
                  [&](const TVector2& A, const TVector2& B) { return comparePoints(A, B, anchor); });

        // Build the convex hull
        std::vector<TVector2> convexHull;
        convexHull.push_back(points[0]);
        convexHull.push_back(points[1]);
        int hullSize = 2;

        for (int i = 2; i < points.size(); i++) {
            while (hullSize >= 2 &&
                   crossProduct(convexHull[hullSize - 2], convexHull[hullSize - 1], points[i]) <= 0) {
                convexHull.pop_back();
                hullSize--;
            }
            convexHull.push_back(points[i]);
            hullSize++;
        }

        return convexHull;
    }

    ClassDef(TRestDetectorExperimentalReadoutPixel, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H
