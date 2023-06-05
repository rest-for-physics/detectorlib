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

void TRestDetectorExperimentalReadoutModule::SetPixels(
    const std::vector<TRestDetectorExperimentalReadoutPixel>& pixels) {
    fPixels = pixels;
    fConvexHull = ComputeConvexHull();

    // search radius is the maximum radius of all pixels
    double searchRadius = 0;
    for (auto& pixel : fPixels) {
        if (pixel.GetRadius() > searchRadius) {
            searchRadius = pixel.GetRadius();
        }
    }
    fSearchRadius = searchRadius;

    BuildKDTree();
}

vector<TVector2> TRestDetectorExperimentalReadoutModule::ComputeConvexHull() {
    std::vector<TVector2> vertices;

    for (auto& pixel : fPixels) {
        for (auto& vertex : pixel.GetVertices()) {
            vertices.push_back(vertex);
        }
    }

    return findConvexHull(vertices);
}

// KD Tree

struct KDNode {
    int index;  // Index of the point in the original collection
    KDNode* left;
    KDNode* right;

    KDNode(int _index) : index(_index), left(nullptr), right(nullptr) {}
};

class KDTree {
   private:
    KDNode* root;
    std::vector<TVector2>& points;

    KDNode* buildTree(std::vector<int>& indices, int start, int end, int depth) {
        if (start > end) {
            return nullptr;
        }

        int axis = depth % 2;  // Split alternately on x and y coordinates
        int mid = (start + end) / 2;

        // Sort the indices based on the current splitting axis
        if (axis == 0) {
            std::sort(indices.begin() + start, indices.begin() + end + 1,
                      [this](int a, int b) { return points[a].X() < points[b].X(); });
        } else {
            std::sort(indices.begin() + start, indices.begin() + end + 1,
                      [this](int a, int b) { return points[a].Y() < points[b].Y(); });
        }

        KDNode* node = new KDNode(indices[mid]);

        node->left = buildTree(indices, start, mid - 1, depth + 1);
        node->right = buildTree(indices, mid + 1, end, depth + 1);

        return node;
    }

    void query(KDNode* node, const TVector2& target, double distance, std::vector<int>& result) {
        if (node == nullptr) {
            return;
        }

        double dist = (points[node->index] - target).Mod();
        if (dist <= distance) {
            result.push_back(node->index);
        }

        int axis = points[node->index].X() < target.X()
                       ? 0
                       : 1;  // Determine the axis to search based on target position

        if (axis == 0) {
            query(node->left, target, distance, result);

            if (target.X() + distance > points[node->index].X()) {
                query(node->right, target, distance, result);
            }
        } else {
            query(node->right, target, distance, result);

            if (target.X() - distance < points[node->index].X()) {
                query(node->left, target, distance, result);
            }
        }
    }

   public:
    KDTree(std::vector<TVector2>& _points) : points(_points) {
        std::vector<int> indices(points.size());
        for (int i = 0; i < points.size(); ++i) {
            indices[i] = i;
        }
        root = buildTree(indices, 0, points.size() - 1, 0);
    }

    std::vector<int> queryIndices(const TVector2& target, double distance) {
        std::vector<int> result;
        query(root, target, distance, result);
        return result;
    }
};

void TRestDetectorExperimentalReadoutModule::BuildKDTree() {
    std::vector<TVector2> centers;
    centers.reserve(fPixels.size());
    for (const auto& pixel : fPixels) {
        centers.push_back(pixel.GetCenter());
    }

    // auto tree = KDTree(centers);

    // delete fKDTree;
    fKDTree = new KDTree(centers);
}

std::vector<TRestDetectorExperimentalReadoutPixel> TRestDetectorExperimentalReadoutModule::GetPixelsInPoint(
    const TVector2& point) const {
    auto indices = fKDTree->queryIndices(point, fSearchRadius);
    cout << "Found " << indices.size() << " pixels in point" << endl;

    std::vector<TRestDetectorExperimentalReadoutPixel> pixels;
    pixels.reserve(indices.size());
    for (auto& index : indices) {
        pixels.push_back(fPixels[index]);
    }

    return pixels;
}

bool TRestDetectorExperimentalReadoutModule::IsInside(const TVector2& point) const {
    return IsPointInsideConvexHull(point, fConvexHull);
}

bool TRestDetectorExperimentalReadoutModule::IsInside(const TVector3& point) const {
    const double z = GetZ(point);
    if (z < 0 || z > fHeight) {
        return false;
    }
    return IsInside(TransformToModuleCoordinates(point));
}

TVector2 TRestDetectorExperimentalReadoutModule::TransformToModuleCoordinates(const TVector3& point) const {
    return {
        (point - fPosition).Dot(fCoordinateAxes.first),
        (point - fPosition).Dot(fCoordinateAxes.second),
    };
}

TVector3 TRestDetectorExperimentalReadoutModule::TransformToAbsoluteCoordinates(const TVector2& point) const {
    return fPosition + point.X() * fCoordinateAxes.first + point.Y() * fCoordinateAxes.second;
}

double TRestDetectorExperimentalReadoutModule::GetZ(const TVector3& point) const {
    (point - fPosition).Dot(fNormal);
}
