#include "TRestDetectorExperimentalReadoutPixel.h"

using namespace std;
using namespace readout;

ClassImp(TRestDetectorExperimentalReadoutPixel);

TRestDetectorExperimentalReadoutPixel::TRestDetectorExperimentalReadoutPixel(
    const std::vector<TVector2>& vertices, unsigned short channel)
    : fChannel(channel) {
    InitializeVertices(vertices);
}

TRestDetectorExperimentalReadoutPixel::TRestDetectorExperimentalReadoutPixel(
    const TVector2& center, const std::pair<double, double>& size, unsigned short channel)
    : TRestDetectorExperimentalReadoutPixel(GetRectangularVertices(center, size), channel) {
    // rectangular pixel
}

TRestDetectorExperimentalReadoutPixel::TRestDetectorExperimentalReadoutPixel(const TVector2& center,
                                                                             double size,
                                                                             unsigned short channel)
    : TRestDetectorExperimentalReadoutPixel(GetRectangularVertices(center, {size, size}), channel) {
    // square pixel
}

void TRestDetectorExperimentalReadoutPixel::InitializeVertices(const std::vector<TVector2>& vertices) {
    // at least three non-aligned vertices
    if (vertices.size() < 3) {
        throw std::invalid_argument("Pixel must have at least three vertices");
    }
    // compute convex hull
    std::vector<TVector2> convexHull = findConvexHull(vertices);
    fVertices = convexHull;

    // compute center and radius
    TVector2 center(0, 0);
    for (auto& vertex : fVertices) {
        center += vertex;
    }
    center *= 1. / double(fVertices.size());
    fCenter = center;

    // the radius is the maximum distance from the center to any vertex
    double radius = 0;
    for (auto& vertex : fVertices) {
        double distance = (vertex - center).Mod();
        if (distance > radius) {
            radius = distance;
        }
    }
    fRadius = radius;
}

std::vector<TVector2> TRestDetectorExperimentalReadoutPixel::GetRectangularVertices(
    const TVector2& center, const std::pair<double, double>& size) {
    // rectangular pixel
    std::vector<TVector2> vertices = {{center.X() - size.first / 2, center.Y() - size.second / 2},
                                      {center.X() + size.first / 2, center.Y() - size.second / 2},
                                      {center.X() + size.first / 2, center.Y() + size.second / 2},
                                      {center.X() - size.first / 2, center.Y() + size.second / 2}};

    return vertices;
}

bool TRestDetectorExperimentalReadoutPixel::IsInside(const TVector2& point) const {
    return readout::IsPointInsideConvexHull(point, fVertices);
}

// tools

namespace readout {
// Helper function to calculate the cross product of two vectors
double crossProduct(const TVector2& A, const TVector2& B, const TVector2& C) {
    return (B.X() - A.X()) * (C.Y() - A.Y()) - (B.Y() - A.Y()) * (C.X() - A.X());
}

// Helper function to calculate the squared distance between two points
double squaredDistance(const TVector2& A, const TVector2& B) {
    double dx = B.X() - A.X();
    double dy = B.Y() - A.Y();
    return dx * dx + dy * dy;
}

// Comparator function to compare two points based on their polar angle with respect to the anchor point
bool comparePoints(const TVector2& A, const TVector2& B, const TVector2& anchor) {
    double cross = crossProduct(anchor, A, B);
    if (cross == 0) {
        // If two points have the same polar angle, choose the one that is closer to the anchor point
        return squaredDistance(anchor, A) < squaredDistance(anchor, B);
    }
    return cross > 0;
}

// Function to find the convex hull of a set of points using the Graham's scan algorithm
std::vector<TVector2> findConvexHull(const std::vector<TVector2>& _points) {
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

bool IsPointInsideConvexHull(const TVector2& point, const std::vector<TVector2>& convexHull) {
    // Iterate through each edge of the convex hull
    for (size_t i = 0; i < convexHull.size(); ++i) {
        const TVector2& vertex1 = convexHull[i];
        const TVector2& vertex2 = convexHull[(i + 1) % convexHull.size()];

        // Calculate the cross product of the point with the current edge
        double crossProductValue = readout::crossProduct(vertex1, vertex2, point);

        // If the cross product is negative for any edge, the point is outside the convex hull
        if (crossProductValue < 0.0) {
            return false;
        }
    }

    // If the point is not outside any edge, it is inside the convex hull
    return true;
}
}  // namespace readout