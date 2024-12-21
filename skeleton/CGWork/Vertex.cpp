#include "Vertex.h"

// Default constructor
Vertex::Vertex()
    : Vector4(), normalStart(Vector4()), normalEnd(Vector4()), hasNormal(false), normalProvidedFromFile(false) {}

// Constructor with position (4 arguments)
Vertex::Vertex(float x, float y, float z, float w)
    : Vector4(x, y, z, w), normalStart(Vector4()), normalEnd(Vector4()), hasNormal(false), normalProvidedFromFile(false) {}

// Constructor with position (3 arguments, defaults w to 1.0f)
Vertex::Vertex(float x, float y, float z)
    : Vector4(x, y, z, 1.0f), normalStart(Vector4()), normalEnd(Vector4()), hasNormal(false), normalProvidedFromFile(false) {}



// Constructor with normal and flag
Vertex::Vertex(const Vector4& position, const Vector4& normalStart, const Vector4& normalEnd, bool normalProvidedFromFile)
    : Vector4(position), normalStart(normalStart), normalEnd(normalEnd), hasNormal(true), normalProvidedFromFile(normalProvidedFromFile) {}


// Getter for normalProvidedFromFile
bool Vertex::isNormalProvidedFromFile() const {
    return normalProvidedFromFile;
}

// Setter for normalProvidedFromFile
void Vertex::setNormalProvidedFromFile(bool provided) {
    normalProvidedFromFile = provided;
    hasNormal = true;
}


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
void Vertex::setNormal(const Vector4& start, const Vector4& end, const bool fromFile) {
    normalStart = start;
    normalEnd = end;
    hasNormal = true;
    normalProvidedFromFile = fromFile;

}

// Update the normal direction relative to the vertex position
void Vertex::updateNormalDirection(const Vector4& direction) {
    normalStart = *this; // The vertex position
    normalEnd = *this + direction.normalize();
    hasNormal = true;

}
