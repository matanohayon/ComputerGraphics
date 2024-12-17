#include "Vector4.h"

// Default constructor
Vector4::Vector4() : x(0), y(0), z(0), w(1) {}

// Constructor with values
Vector4::Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

// Vector addition
Vector4 Vector4::operator+(const Vector4& v) const {
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

// Vector subtraction
Vector4 Vector4::operator-(const Vector4& v) const {
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

// Scalar multiplication
Vector4 Vector4::operator*(double scalar) const {
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

// Scalar division
Vector4 Vector4::operator/(double scalar) const {
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}


// Dot product
double Vector4::dot(const Vector4& v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

// Cross product
Vector4 Vector4::cross(const Vector4& v) const {
    return Vector4(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x,
        1
    );
}

// Magnitude of the vector
double Vector4::magnitude() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

// Normalize the vector
Vector4 Vector4::normalize() const {
    double mag = magnitude();
    return Vector4(x / mag, y / mag, z / mag, w / mag);
}

// Distance between two vectors
double Vector4::distance(const Vector4& v) const {
    return (*this - v).magnitude();
}

// Set coordinates
void Vector4::set(double newX, double newY, double newZ) {
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
double Vector4::angle(const Vector4& v) const {
    double dotProduct = dot(v);
    double magnitudes = magnitude() * v.magnitude();
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
