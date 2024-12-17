#ifndef Vector4_H
#define Vector4_H

#include <cmath>
#include <iostream>
#include <algorithm> // For std::min, std::max

class Vector4 {
public:
    double x, y, z,w;

    // Constructors
    Vector4();
    Vector4(double x, double y, double z, double w=1);

    // Vector operations
    Vector4 operator+(const Vector4& v) const;
    Vector4 operator-(const Vector4& v) const;
    Vector4 operator*(double scalar) const;
    Vector4 operator/(double scalar) const;
    double dot(const Vector4& v) const;
    Vector4 cross(const Vector4& v) const;
    double magnitude() const;
    Vector4 normalize() const;

    // Utility functions
    double distance(const Vector4& v) const;
    void set(double newX, double newY, double newZ);
    void updateMin(const Vector4& v);
    void updateMax(const Vector4& v);
    static Vector4 zero();

    // Angle between vectors
    double angle(const Vector4& v) const;

    // Equality and inequality
    bool operator==(const Vector4& v) const;
    bool operator!=(const Vector4& v) const;
};

#endif // Vector4_H
