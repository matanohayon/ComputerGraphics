#ifndef VERTEX_H
#define VERTEX_H

#include "Vector4.h"

class Vertex : public Vector4 {
private:
    Vector4 normalStart;  // Start point of the normal
    Vector4 normalEnd;    // End point of the normal
    bool hasNormal;
    bool normalProvidedFromFile; // Flag to indicate if the normal was provided from a file


public:
    // Constructors
 // Default constructor
    Vertex();

    // Constructor with position (4 arguments)
    Vertex(float x, float y, float z, float w);

    // Constructor with position (3 arguments, default w to 1.0f)
    Vertex(float x, float y, float z);

    // Constructor with normal and flag
    Vertex(const Vector4& position, const Vector4& normalStart, const Vector4& normalEnd, bool normalProvidedFromFile);


    // Assignment operator from Vector4
    Vertex& operator=(const Vector4& v);

    // Getters
    const Vector4& getNormalStart() const;
    const Vector4& getNormalEnd() const;
    bool getHasNormal() const;

    // Setters
    void setNormalStart(const Vector4& start);
    void setNormalEnd(const Vector4& end);
    void setNormal(const Vector4& start, const Vector4& end, const bool fromFile);

    // Convenience methods
    void updateNormalDirection(const Vector4& direction); // Updates normal direction relative to position

    bool isNormalProvidedFromFile() const;
    void setNormalProvidedFromFile(bool provided);

};

#endif // VERTEX_H
