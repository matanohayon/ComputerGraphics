#include "Vertex.h"

// Default constructor
Vertex::Vertex() : Vector4(), normalStart(Vector4()), normalEnd(Vector4()), hasNormal(false) {}

// Constructor with position
Vertex::Vertex(double x, double y, double z, double w)
    : Vector4(x, y, z, w), normalStart(Vector4()), normalEnd(Vector4()), hasNormal(false) {}

Vertex& Vertex::operator=(const Vector4& v) {
    // Assign Vector4 components (position)
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;

    // Keep existing normal values unchanged
    return *this;
}


// Getter for normal start
const Vector4& Vertex::getNormalStart() const {
    return normalStart;
}

// Getter for normal end
const Vector4& Vertex::getNormalEnd() const {
    return normalEnd;
}

bool Vertex::getHasNormal() const {
    return hasNormal;
}

// Setter for normal start
void Vertex::setNormalStart(const Vector4& start) {
    normalStart = start;
}

// Setter for normal end
void Vertex::setNormalEnd(const Vector4& end) {
    normalEnd = end;
}

// Setter for normal start and end
void Vertex::setNormal(const Vector4& start, const Vector4& end) {
    normalStart = start;
    normalEnd = end;
    hasNormal = true;

}

// Update the normal direction relative to the vertex position
void Vertex::updateNormalDirection(const Vector4& direction) {
    normalStart = *this; // The vertex position
    normalEnd = *this + direction.normalize();
    hasNormal = true;

}
