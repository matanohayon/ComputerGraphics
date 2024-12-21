#pragma once
#ifndef MATRIX4_H
#define MATRIX4_H
#include "Vector4.h"
#include <Eigen/Dense> //for inverting the matrix of the normal vector only!

class Matrix4 {
public:
    float m[4][4]; // 4x4 matrix

    // Constructor
    Matrix4();
    //constructor from vectors
    Matrix4(Vector4& a, Vector4& b, Vector4& c, Vector4& d);

    // Matrix multiplication
    Matrix4 operator*(const Matrix4& other) const;

    // Vector transformation
    Vector4 transform(const Vector4& v) const;

    // Static methods for creating transformation matrices
    static Matrix4 translate(float dx, float dy, float dz);
    static Matrix4 scale(float sx, float sy, float sz);
    static Matrix4 rotateX(float angle);
    static Matrix4 rotateY(float angle);
    static Matrix4 rotateZ(float angle);

    Matrix4 withoutTranslation() const;

    // Projection matrices
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 perspective(float fov, float aspect, float near, float far, float d);

    // Modify perspective parameter 'd'
    void setPerspectiveD(float d);

    // Transpose function
    Matrix4 transpose() const;

    // Inverse function
    Matrix4 inverse() const;
    // Print the matrix (for debugging)
    void print() const;
    double DegsToRad(double angle);

};

#endif // MATRIX4_H
