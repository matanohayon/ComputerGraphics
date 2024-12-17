#include "Poly.h"
#include <iostream>

Poly::Poly()
    : hasPolyNormal(false),
    hasVertexNormal(false),
    color(RGB(255, 255, 255)),
    polyNormal(PolyNormal()) {}

void Poly::addVertex(const Vector4& vertex) {
    vertices.push_back(vertex);
}

void Poly::addVertexNormal(const VertexNormal& normal) {
    vertexNormals.push_back(normal);
    hasVertexNormal = true;
}

std::vector<Vector4>& Poly::getVertices() {
    return vertices;
}

const std::vector<Vector4>& Poly::getVertices() const {
    return vertices;
}

std::vector<VertexNormal>& Poly::getVertexNormals() {
    return vertexNormals;
}

const std::vector<VertexNormal>& Poly::getVertexNormals() const {
    return vertexNormals;
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

bool Poly::hasVertexNormalsDefined() const {
    return hasVertexNormal;
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

void Poly::applyTransform(const Matrix4& transform) {
    /*
    for (Vector4& vertex : vertices) {
        vertex = transform.transform(vertex);
    }*/
    /*
    for (VertexNormal& normal : vertexNormals) {
        normal.start = transform.transform(normal.start);
        normal.end = transform.transform(normal.end);
    }

    if (hasPolyNormal) {
        polyNormal.start = transform.transform(polyNormal.start);
        polyNormal.end = transform.transform(polyNormal.end);
    }*/
}

void Poly::calculateBoundingBox(Vector4& min, Vector4& max) const {
    min = Vector4(DBL_MAX, DBL_MAX, DBL_MAX, 1.0);
    max = Vector4(DBL_MIN, DBL_MIN, DBL_MIN, 1.0);

    for (const Vector4& vertex : vertices) {
        min.updateMin(vertex);
        max.updateMax(vertex);
    }
}
