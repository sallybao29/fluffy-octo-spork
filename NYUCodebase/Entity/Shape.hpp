#ifndef Shape_hpp
#define Shape_hpp

#include <stdio.h>
#include <vector>
#include "Vector3.hpp"

/*!
 * @discussion An abstract class that defines a shape
 */
class Shape {
public:
    Shape(float width = 1.0f, float height = 1.0f);
    
    /*!
     * @discussion Gets the main vertices of the shape
     * @return A vector of vertices as (x, y, z) triples
     */
    const std::vector<Vector3>& points() const;
    
    /*!
     * @discussion Gets the vertices of each triangle that composes the shape
     * @return A vector of vertices. Must be read as pairs
     */
    const std::vector<float> triangles() const;
    
    /*!
     * @discussion Resizes the shape
     * @param width The new width of the shape
     * @param height The new height of the shape
     */
    virtual void SetSize(float width, float height) = 0;
    
    /*!
     * @discussion Dynamically creates a new shape based on the properties of this one
     * @return A dynamic clone of this shape
     */
    virtual Shape* Clone() = 0;
    virtual ~Shape();
protected:
    /// The width and height of the shape
    Vector3 size;
    
    /// The main vertices of this shape
    std::vector<Vector3> vertices;
    /// The vertices of each triangle that composes this shape. Counter-clockwise order
    std::vector<float> triangleVertices;
};

class Rectangle : public Shape {
public:
    Rectangle(float width = 1.0f, float height = 1.0f);
    void SetSize(float width, float height) override;
    Rectangle* Clone() override;
};

class Triangle : public Shape {
    Triangle(float width = 1.0f, float height = 1.0f);
    void SetSize(float width, float height) override;
    Triangle* Clone() override;
};

#endif
