#include "Poly.h"
#include <cfloat>

Poly::Poly()
    : hasPolyNormalFromFile(false),
    hasPolyNormalCalculated(false),
    color(RGB(255, 255, 255)),
    polyNormalFromFile(Normal()),
    polyNormalCalculated(Normal()) {}


void Poly::flipNormals() {
    // Flip vertex normals
    for (auto& vertex : vertices) {
        vertex.flipNormals();
    }

    // Flip polygon normals
    polyNormalFromFile.flip();
    polyNormalCalculated.flip();
}

void Poly::addVertex(const Vertex& vertex) {
    vertices.push_back(vertex);
}

std::vector<Vertex>& Poly::getVertices() {
    return vertices;
}

const std::vector<Vertex>& Poly::getVertices() const {
    return vertices;
}

const Normal& Poly::getPolyNormalFromFile() const {
    return polyNormalFromFile;
}

const Normal& Poly::getPolyNormalCalculated() const {
    return polyNormalCalculated;
}

void Poly::setPolyNormalFromFile(const Normal& normal) {
    polyNormalFromFile = normal;
    hasPolyNormalFromFile = true;
}

void Poly::setPolyNormalCalculated(const Normal& normal) {
    polyNormalCalculated = normal;
    hasPolyNormalCalculated = true;
}

void Poly::calculatePolyNormal(const Vector4& centroid, const Vector4& direction) {
    polyNormalCalculated = Normal(centroid, centroid + direction.normalize());
    hasPolyNormalCalculated = true;
}

bool Poly::hasPolyNormalFromFileDefined() const {
    return hasPolyNormalFromFile;
}

bool Poly::hasPolyNormalCalculatedDefined() const {
    return hasPolyNormalCalculated;
}

size_t Poly::getVertexCount() const {
    return vertices.size();
}

void Poly::setColor(COLORREF c) {
    color = c;
}

COLORREF Poly::getColor() const {
    return color;
}

void Poly::calculateBoundingBox(Vector4& min, Vector4& max) const {
    min = Vector4(FLT_MAX, FLT_MAX, FLT_MAX, 1.0);
    max = Vector4(FLT_MIN, FLT_MIN, FLT_MIN, 1.0);

    for (const Vertex& vertex : vertices) {
        min.updateMin(vertex);
        max.updateMax(vertex);
    }
}

#include "Poly.h"

// Apply a transformation to the polygon, including vertices and normals
void Poly::applyTransform(const Matrix4& transform, const Matrix4& normalTransform) {
    // Transform all vertices
    for (Vertex& vertex : vertices) {
        vertex.applyTransform(transform, normalTransform);
    }

    // Transform polygon normal from file
    if (hasPolyNormalFromFile) {
        polyNormalFromFile.transform(transform, normalTransform);
    }

    // Transform polygon calculated normal
    if (hasPolyNormalCalculated) {
        polyNormalCalculated.transform(transform, normalTransform);
    }
}

const Normal& Poly::getNormal() const {
    if (hasPolyNormalFromFile)
        return polyNormalFromFile;
    return polyNormalCalculated;
}
