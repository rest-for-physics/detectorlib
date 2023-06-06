/**
 * @file TRestDetectorExperimentalReadoutPixel.h
 * @brief Defines the TRestDetectorExperimentalReadoutPixel class.
 */

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H

#include <TVector2.h>
#include <TVector3.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

/**

 * @class TRestDetectorExperimentalReadoutPixel
 * @brief Represents a pixel in an experimental readout detector.
 * @author Luis Obis

*/
class TRestDetectorExperimentalReadoutPixel {
   private:
    std::vector<TVector2> fVertices; /**< The vertices of the pixel. */
    TVector2 fCenter; /**< The center of the pixel in mm. A pixel is guaranteed to be inside a circle with
                         center fCenter and radius fRadius. */
    double fRadius;   /**< The radius of the pixel in mm. A pixel is guaranteed to be inside a circle with
                         center fCenter and radius fRadius. */

    /**

    @brief Initializes the vertices of the pixel.
    @param vertices The vertices of the pixel.
    */
    void InitializeVertices(const std::vector<TVector2>& vertices);
    /**

    @brief Calculates the vertices of a rectangular pixel.
    @param center The center of the pixel.
    @param size The size of the pixel (width, height).
    @return The vertices of the rectangular pixel.
    */
    static std::vector<TVector2> GetRectangularVertices(const TVector2& center,
                                                        const std::pair<double, double>& size);

   public:
    /**

    @brief Constructs a TRestDetectorExperimentalReadoutPixel object with given vertices.
    @param vertices The vertices of the pixel.
    */
    explicit TRestDetectorExperimentalReadoutPixel(const std::vector<TVector2>& vertices);
    /**

    @brief Constructs a rectangular TRestDetectorExperimentalReadoutPixel object.
    @param center The center of the pixel.
    @param size The size of the pixel (width, height).
    */
    TRestDetectorExperimentalReadoutPixel(const TVector2& center, const std::pair<double, double>& size);
    /**

    @brief Constructs a square TRestDetectorExperimentalReadoutPixel object.
    @param center The center of the pixel.
    @param size The size of the pixel (width and height).
    */
    TRestDetectorExperimentalReadoutPixel(const TVector2& center, double size);
    // needed for root
    TRestDetectorExperimentalReadoutPixel() = default;
    virtual ~TRestDetectorExperimentalReadoutPixel() = default;

    /**

    @brief Returns the center of the pixel.
    @return The center of the pixel.
    */
    TVector2 GetCenter() const { return fCenter; }
    /**

    @brief Returns the radius of the pixel.
    @return The radius of the pixel.
    */
    double GetRadius() const { return fRadius; }
    /**

    @brief Returns the vertices of the pixel.
    @return The vertices of the pixel.
    */
    std::vector<TVector2> GetVertices() const { return fVertices; }
    /**

    @brief Checks if a given point is inside the pixel.
    @param point The point to check.
    @return True if the point is inside the pixel, false otherwise.
    */
    bool IsInside(const TVector2& point) const;
    ClassDef(TRestDetectorExperimentalReadoutPixel, 1);
};

namespace readout {

/**

@brief Calculates the triple product of three vectors.
@param A The first vector.
@param B The second vector.
@param C The third vector.
@return The cross product of the vectors.
*/
double crossProduct(const TVector2& A, const TVector2& B, const TVector2& C);
/**

@brief Calculates the squared distance between two points.
@param A The first point.
@param B The second point.
@return The squared distance between the points.
*/
double squaredDistance(const TVector2& A, const TVector2& B);
/**

@brief Comparator function to compare two points based on their polar angle with respect to the anchor point.
@param A The first point.
@param B The second point.
@param anchor The anchor point.
@return True if point A is smaller than point B, false otherwise.
*/
bool comparePoints(const TVector2& A, const TVector2& B, const TVector2& anchor);
/**

@brief Finds the convex hull of a set of points using the Graham's scan algorithm.
@param _points The input points.
@return The convex hull of the points.
*/
std::vector<TVector2> findConvexHull(const std::vector<TVector2>& _points);
/**

@brief Checks if a given point is inside a convex hull.
@param point The point to check.
@param convexHull The convex hull.
@return True if the point is inside the convex hull, false otherwise.
*/
bool IsPointInsideConvexHull(const TVector2& point, const std::vector<TVector2>& convexHull);
}  // namespace readout
#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTPIXEL_H