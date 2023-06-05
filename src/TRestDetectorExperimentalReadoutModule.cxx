#include "TRestDetectorExperimentalReadoutModule.h"

using namespace std;
using namespace readout;

ClassImp(TRestDetectorExperimentalReadoutModule);

TRestDetectorExperimentalReadoutModule::TRestDetectorExperimentalReadoutModule(const TVector3& position,
                                                                               double height,
                                                                               const TVector3& normal,
                                                                               double rotationInDegrees) {
    fPosition = position;
    fHeight = height;
    fNormal = normal;

    fCoordinateAxes = {{1, 0, 0}, {0, 1, 0}};

    // rotate the coordinate axes by rotationInDegrees around the normal vector
    fCoordinateAxes.first.Rotate(rotationInDegrees * TMath::RadToDeg(), fNormal);
    fCoordinateAxes.second.Rotate(rotationInDegrees * TMath::RadToDeg(), fNormal);
}

void TRestDetectorExperimentalReadoutModule::InsertPixel(const TRestDetectorExperimentalReadoutPixel& pixel) {
    fPixels.push_back(pixel);
}

std::vector<TVector2> TRestDetectorExperimentalReadoutModule::GetConvexHull() const {
    std::vector<TVector2> vertices;

    for (auto& pixel : fPixels) {
        for (auto& vertex : pixel.GetVertices()) {
            vertices.push_back(vertex);
        }
    }

    return findConvexHull(vertices);
}