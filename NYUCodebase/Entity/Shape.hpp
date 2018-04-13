#ifndef Shape_hpp
#define Shape_hpp

#include <stdio.h>
#include <vector>
#include "Vector3.hpp"

class Shape {
public:
    Shape(float width = 1.0f, float height = 1.0f);
    const std::vector<Vector3>& points() const;
    const std::vector<float> triangles() const;
    virtual void SetSize(float width, float height) = 0;
    virtual Shape* Clone() = 0;
    virtual ~Shape();
protected:
    Vector3 size;
    std::vector<Vector3> vertices;
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
