#ifndef POLY_H
#define POLY_H

#include <vector>
#include "Vector4.h"
#include "Matrix4.h"
#include <Windows.h> // For COLORREF

// Represents the polygon normal with a start and end point
class PolyNormal {
public:
    Vector4 start;
    Vector4 end;

    PolyNormal() : start(Vector4()), end(Vector4()) {}
    PolyNormal(const Vector4& start, const Vector4& end) : start(start), end(end) {}
};

// Represents a vertex normal with a start and end point
class VertexNormal {
public:
    Vector4 start;
    Vector4 end;

    VertexNormal() : start(Vector4()), end(Vector4()) {}
    VertexNormal(const Vector4& start, const Vector4& end) : start(start), end(end) {}
};

// Represents a polygon in the scene
class Poly {
private:
    std::vector<Vector4> vertices;
    std::vector<VertexNormal> vertexNormals;
    PolyNormal polyNormal;
    bool hasPolyNormal;
    bool hasVertexNormal;
    COLORREF color;

public:
    Poly();

    void addVertex(const Vector4& vertex);
    void addVertexNormal(const VertexNormal& normal);

    std::vector<Vector4>& getVertices();
    const std::vector<Vector4>& getVertices() const;

    std::vector<VertexNormal>& getVertexNormals();
    const std::vector<VertexNormal>& getVertexNormals() const;

    const PolyNormal& getPolyNormal() const;

    void setPolyNormal(const PolyNormal& normal);
    void calculatePolyNormal(const Vector4& centroid, const Vector4& direction);

    bool hasPolyNormalDefined() const;
    bool hasVertexNormalsDefined() const;

    size_t getVertexCount() const;

    void setColor(COLORREF c);
    COLORREF getColor() const;

    void applyTransform(const Matrix4& transform);

    void calculateBoundingBox(Vector4& min, Vector4& max) const;

    void print() const;
};

#endif // POLY_H
