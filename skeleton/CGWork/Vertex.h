#ifndef VERTEX_H
#define VERTEX_H

#include "Vector4.h"

class Vertex : public Vector4 {
private:
    Vector4 normalStart;  // Start point of the normal
    Vector4 normalEnd;    // End point of the normal
    bool hasNormal;

public:
    // Constructors
    Vertex(); // Default constructor
    Vertex(float x, float y, float z, float w = 1.0); // Constructor with position
    Vertex(const Vector4& position, const Vector4& normal); // Constructor with position and normal
    Vertex(const Vector4& position, const Vector4& start, const Vector4& end); // Full constructor

    // Assignment operator from Vector4
    Vertex& operator=(const Vector4& v);

    // Getters
    const Vector4& getNormalStart() const;
    const Vector4& getNormalEnd() const;
    bool getHasNormal() const;

    // Setters
    void setNormalStart(const Vector4& start);
    void setNormalEnd(const Vector4& end);
    void setNormal(const Vector4& start, const Vector4& end);

    // Convenience methods
    void updateNormalDirection(const Vector4& direction); // Updates normal direction relative to position
};

#endif // VERTEX_H
