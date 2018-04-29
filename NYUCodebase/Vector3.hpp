#ifndef Vector3_hpp
#define Vector3_hpp

#include <utility>
#include <stdio.h>
#include "Matrix.h"

/*!
 * @abstract Represents a vector of length 3
 */
class Vector3 {
public:
    Vector3();
    Vector3(float x, float y, float z = 0.0f);
    
    /*!
     * @discussion Calculates the length of the vector
     * @return The length of the vector
     */
    float Length() const;
    
    /*!
     * @abstract Normalizes the vector
     * @discussion Converts this vector into a unit vector by dividing its components by the vector's length
     */
    void Normalize();
    
    /*!
     * @discussion Implicitly converts a Vector3 to a std::pair<float, float>
     */
    operator std::pair<float, float>() const;
    
    float x;
    float y;
    float z;
};

/*!
 * @discussion Implements multiplication between a matrix and a vector
 * @remark The vector is treated as a 4 x 1 homogeneous coordinate matrix
 * @param matrix A 4 x 4 transformation matrix. The first operand
 * @param vector A vector of length 3. The second operand
 * @return The resultant vector
 */
Vector3 operator * (const Matrix& matrix, const Vector3& vector);

/*!
 * @discussion Calculates the distance between two position vectors
 * @param v1 First position
 * @param v2 Second position
 * @return The distance between the two positions
 */
float distance(const Vector3& v1, const Vector3& v2);

#endif
