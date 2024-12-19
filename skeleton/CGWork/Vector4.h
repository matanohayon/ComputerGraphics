#ifndef Vector4_H
#define Vector4_H

#include <cmath>
#include <iostream>
#include <algorithm> // For std::min, std::max

class Vector4 {
public:
    float x, y, z,w;

    // Constructors
    Vector4();
    Vector4(float x, float y, float z, float w=1);

    // Vector operations
    Vector4 operator+(const Vector4& v) const;
    Vector4 operator-(const Vector4& v) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;
    float dot(const Vector4& v) const;
    Vector4 cross(const Vector4& v) const;
    float magnitude() const;
    Vector4 normalize() const;

    // Utility functions
    float distance(const Vector4& v) const;
    void set(float newX, float newY, float newZ);
    void updateMin(const Vector4& v);
    void updateMax(const Vector4& v);
    static Vector4 zero();

    // Angle between vectors
    float angle(const Vector4& v) const;

    // Equality and inequality
    bool operator==(const Vector4& v) const;
    bool operator!=(const Vector4& v) const;
};

#endif // Vector4_H
