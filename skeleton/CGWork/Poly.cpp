#include "Poly.h"
#include <iostream>

Poly::Poly()
    : hasPolyNormal(false),
    color(RGB(255, 255, 255)),
    polyNormal(PolyNormal()) {}

void Poly::addVertex(const Vertex& vertex) {
    vertices.push_back(vertex);
}



std::vector<Vertex>& Poly::getVertices() {
    return vertices;
}

const std::vector<Vertex>& Poly::getVertices() const {
    return vertices;
}





const PolyNormal& Poly::getPolyNormal() const {
    return polyNormal;
}

void Poly::setPolyNormal(const PolyNormal& normal) {
    polyNormal = normal;
    hasPolyNormal = true;
}

void Poly::calculatePolyNormal(const Vector4& centroid, const Vector4& direction) {
    polyNormal = PolyNormal(centroid, centroid + direction.normalize());
    hasPolyNormal = true;
}

bool Poly::hasPolyNormalDefined() const {
    return hasPolyNormal;
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
