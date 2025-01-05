#ifndef POLY_H
#define POLY_H

#include <vector>
#include "Normal.h"
#include "Vertex.h"
#include <Windows.h> // For COLORREF

class Poly {
private:
    std::vector<Vertex> vertices;  // List of vertices
    Normal polyNormalFromFile;     // Normal provided from a file
    Normal polyNormalCalculated;  // Normal calculated programmatically
    bool hasPolyNormalFromFile;
    bool hasPolyNormalCalculated;
    COLORREF color;

public:
    Poly();

    void addVertex(const Vertex& vertex);
    std::vector<Vertex>& getVertices();
    const std::vector<Vertex>& getVertices() const;

    const Normal& getPolyNormalFromFile() const;
    const Normal& getPolyNormalCalculated() const;
    const Normal& getNormal() const;

    void setPolyNormalFromFile(const Normal& normal);
    void setPolyNormalCalculated(const Normal& normal);
    void calculatePolyNormal(const Vector4& centroid, const Vector4& direction);

    void flipNormals();

    bool hasPolyNormalFromFileDefined() const;
    bool hasPolyNormalCalculatedDefined() const;

    size_t getVertexCount() const;

    void setColor(COLORREF c);
    COLORREF getColor() const;
    void applyTransform(const Matrix4& transform, const Matrix4& normalTransform);

    void calculateBoundingBox(Vector4& min, Vector4& max) const;
};

#endif // POLY_H
