#ifndef POLY_H
#define POLY_H

#include <vector>
#include "Vector4.h"
#include "Matrix4.h"
#include "Vertex.h"
#include <Windows.h> // For COLORREF

// Represents the polygon normal with a start and end point
class PolyNormal {
public:
    Vector4 start;
    Vector4 end;
    bool wasProvidedFromFile;

    // Default constructor
    PolyNormal() : start(Vector4()), end(Vector4()), wasProvidedFromFile(false) {}

    // Constructor with start and end points
    PolyNormal(const Vector4& start, const Vector4& end, bool wasProvidedFromFile = false)
        : start(start), end(end), wasProvidedFromFile(wasProvidedFromFile) {}
};


// Represents a polygon in the scene
class Poly {
private:
    std::vector<Vertex> vertices;  // Use Vertex instead of Vector4
    PolyNormal polyNormal;
    bool hasPolyNormal;
    COLORREF color;

public:
    Poly();

    void addVertex(const Vertex& vertex);

    std::vector<Vertex>& getVertices();
    const std::vector<Vertex>& getVertices() const;


    const PolyNormal& getPolyNormal() const;

    void setPolyNormal(const PolyNormal& normal);
    void calculatePolyNormal(const Vector4& centroid, const Vector4& direction);

    bool hasPolyNormalDefined() const;

    size_t getVertexCount() const;

    void setColor(COLORREF c);
    COLORREF getColor() const;

    void calculateBoundingBox(Vector4& min, Vector4& max) const;

};

#endif // POLY_H
