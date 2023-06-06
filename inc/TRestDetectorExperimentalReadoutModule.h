/**
 * @file TRestDetectorExperimentalReadoutModule.h
 * @brief Defines the TRestDetectorExperimentalReadoutModule class.
 */

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H

#include <map>

#include "TRestDetectorExperimentalReadoutPixel.h"

class KDTree;

/**
 * @class TRestDetectorExperimentalReadoutModule
 * @brief Represents a module in an experimental readout detector.
 * @author Luis Obis
 */
class TRestDetectorExperimentalReadoutModule {
   private:
    std::vector<TRestDetectorExperimentalReadoutPixel> fPixels; /**< The pixels of the module. */

    TVector3 fPosition;                            /**< The position of the module in 3D space. */
    TVector3 fNormal;                              /**< The normal vector to the module. */
    double fHeight;                                /**< The height of the module (drift distance). */
    std::pair<TVector3, TVector3> fCoordinateAxes; /**< The coordinate axes of the module. */

    // auxiliary data structures
    // std::unique_ptr<MyKDTree> fKDTree;
    KDTree* fKDTree = nullptr;         /**< The KDTree used for spatial queries. */
    double fSearchRadius = 0;          /**< The search radius for spatial queries. */
    std::vector<TVector2> fConvexHull; /**< The convex hull of the module. */

    /**
     * @brief Builds the KDTree for the module.
     */
    void BuildKDTree();

    /**
     * @brief Computes the convex hull of the module.
     * @return The convex hull of the module.
     */
    std::vector<TVector2> ComputeConvexHull();

   public:
    /**
     * @brief Constructs a TRestDetectorExperimentalReadoutModule object with the specified parameters.
     * @param position The position of the module in 3D space.
     * @param height The height of the module (drift distance).
     * @param normal The normal vector to the module.
     * @param rotationInDegrees The rotation angle of the module in degrees (default: 0).
     */
    TRestDetectorExperimentalReadoutModule(const TVector3& position, double height, const TVector3& normal,
                                           double rotationInDegrees = 0);

    // needed for root
    TRestDetectorExperimentalReadoutModule() = default;
    virtual ~TRestDetectorExperimentalReadoutModule() {
        // delete fKDTree;
    }

    /**
     * @brief Returns the number of pixels in the module.
     * @return The number of pixels.
     */
    size_t GetNumberOfPixels() const { return fPixels.size(); }

    /**
     * @brief Returns the pixels of the module.
     * @return The pixels.
     */
    std::vector<TRestDetectorExperimentalReadoutPixel> GetPixels() const { return fPixels; }

    /**
     * @brief Sets the pixels of the module.
     * @param pixels The pixels to set.
     */
    void SetPixels(const std::vector<TRestDetectorExperimentalReadoutPixel>& pixels);

    /**
     * @brief Returns the convex hull of the module.
     * @return The convex hull.
     */
    std::vector<TVector2> GetConvexHull() const { return fConvexHull; }

    /**
     * @brief Returns the pixels that contain a given point.
     * @param point The point to check.
     * @return The pixels containing the point.
     */
    std::vector<TRestDetectorExperimentalReadoutPixel> GetPixelsInPoint(const TVector2& point) const;

    /**
     * @brief Returns the Z coordinate of a given point inside the module.
     * @param point The point to check.
     * @return The Z coordinate of the point.
     */
    double GetZ(const TVector3& point) const;

    /**
     * @brief Checks if a given point is inside the module in the module's local XY plane.
     * @param point The point to check.
     * @return True if the point is inside the module, false otherwise.
     */
    bool IsInside(const TVector2& point) const;

    /**
     * @brief Checks if a given point is inside the module in 3D space.
     * @param point The point to check.
     * @return True if the point is inside the module, false otherwise.
     */
    bool IsInside(const TVector3& point) const;

    /**
     * @brief Transforms a point from 3D space to the module's local XY plane coordinates.
     * @param point The point to transform.
     * @return The transformed point in the local XY plane.
     */
    TVector2 TransformToModuleCoordinates(const TVector3& point) const;

    /**
     * @brief Transforms a point from the module's local XY plane coordinates to 3D space.
     * @param point The point to transform.
     * @return The transformed point in 3D space.
     */
    TVector3 TransformToAbsoluteCoordinates(const TVector2& point) const;

    ClassDef(TRestDetectorExperimentalReadoutModule, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTMODULE_H
