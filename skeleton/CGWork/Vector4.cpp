#include "Vector4.h"

// Default constructor
Vector4::Vector4() : x(0), y(0), z(0), w(1) {}

// Constructor with values
Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

// Vector addition
Vector4 Vector4::operator+(const Vector4& v) const {
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

// Vector subtraction
Vector4 Vector4::operator-(const Vector4& v) const {
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

// Scalar multiplication
Vector4 Vector4::operator*(float scalar) const {
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

// Scalar division
Vector4 Vector4::operator/(float scalar) const {
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}


// Dot product
float Vector4::dot(const Vector4& v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

// Cross product
/*
Why Should w = 0?

In homogeneous coordinates (4D vectors), the w component represents a distinction between points (w = 1) and directions (w = 0):
Points have a w component of 1, indicating a position in space.
Directions (vectors) have a w component of 0, meaning they represent direction and magnitude but no fixed position.
The result of a cross product is a direction (normal vector) and not a point, so its w component must be set to 0.
*/
Vector4 Vector4::cross(const Vector4& v) const {
    return Vector4(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x,
        0
    );
}

// Magnitude of the vector should exclude w 
/*
The w component is not part of the spatial dimensions (x, y, z); it serves as a scaling factor for the vector in 4D space.
Including w in the magnitude calculation would distort the result, making the length of a purely spatial vector incorrect.
*/
float Vector4::magnitude() const {
    return std::sqrt(x * x + y * y + z * z);
}

// Normalize the vector
Vector4 Vector4::normalize() const {
    float mag = magnitude();
    if (mag == 0) {
        return Vector4(0, 0, 0, w); // Return a zero vector with the same w component
    }
    return Vector4(x / mag, y / mag, z / mag, w); // Preserve the w component during normalization
}

// Distance between two vectors
float Vector4::distance(const Vector4& v) const {
    return (*this - v).magnitude();
}

// Set coordinates
void Vector4::set(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

// Update minimum values
void Vector4::updateMin(const Vector4& v) {
    x = std::min(x, v.x);
    y = std::min(y, v.y);
    z = std::min(z, v.z);
}

// Update maximum values
void Vector4::updateMax(const Vector4& v) {
    x = std::max(x, v.x);
    y = std::max(y, v.y);
    z = std::max(z, v.z);
}

// Zero vector
Vector4 Vector4::zero() {
    return Vector4(0.0, 0.0, 0.0);
}

// Angle between two vectors
float Vector4::angle(const Vector4& v) const {
    float dotProduct = dot(v);
    float magnitudes = magnitude() * v.magnitude();
    return magnitudes > 0 ? std::acos(dotProduct / magnitudes) : 0.0;
}

// Equality comparison
bool Vector4::operator==(const Vector4& v) const {
    return x == v.x && y == v.y && z == v.z;
}

// Inequality comparison
bool Vector4::operator!=(const Vector4& v) const {
    return !(*this == v);
}
