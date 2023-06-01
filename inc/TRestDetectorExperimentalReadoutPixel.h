//
// Created by lobis on 01-Jun-23.
//

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H

#include <TVector2.h>
#include <TVector3.h>

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
        // check the vertices form a convex polygon (TODO)

        fVertices = vertices;
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

   public:
    explicit TRestDetectorExperimentalReadoutPixel(const std::vector<TVector2>& vertices) { InitializeVertices(vertices); }

    TRestDetectorExperimentalReadoutPixel(const TVector2& center, const std::pair<double, double>& size) {
        // rectangular pixel
        InitializeVertices(GetRectangularVertices(center, size));
    }

    TRestDetectorExperimentalReadoutPixel(const TVector2& center, double size) {
        // square pixel
        InitializeVertices(GetRectangularVertices(center, {size, size}));
    }

    ~TRestDetectorExperimentalReadoutPixel() = default;

    TVector2 GetCenter() const {
        TVector2 center(0, 0);
        for (auto& vertex : fVertices) {
            center += vertex;
        }
        center *= 1. / fVertices.size();
        return center;
    }

    std::vector<TVector2> GetVertices() const { return fVertices; }

    ClassDef(TRestDetectorExperimentalReadoutPixel, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H
