#include "Matrix4.h"
#include "Vector4.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

// Constructor: Initializes to identity matrix
Matrix4::Matrix4() {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = (i == j) ? 1.0 : 0.0;
}

//construction from vectors
Matrix4::Matrix4(Vector4& a, Vector4& b, Vector4& c, Vector4& d)
{
    m[0][0] = a.x;
    m[0][1] = a.y;
    m[0][2] = a.z;
    m[0][3] = a.w;

    m[1][0] = b.x;
    m[1][1] = b.y;
    m[1][2] = b.z;
    m[1][3] = b.w;

    m[2][0] = c.x;
    m[2][1] = c.y;
    m[2][2] = c.z;
    m[2][3] = c.w;

    m[3][0] = d.x;
    m[3][1] = d.y;
    m[3][2] = d.z;
    m[3][3] = d.w;
}


// Matrix multiplication
Matrix4 Matrix4::operator*(const Matrix4& other) const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

// Transform a Vector4
Vector4 Matrix4::transform(const Vector4& v) const {
    double newX = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    double newY = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    double newZ = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    double newW = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

    // If w is not 1, normalize the result
    if (newW != 1.0 && newW != 0.0) {
        newX /= newW;
        newY /= newW;
        newZ /= newW;
    }

    return Vector4(newX, newY, newZ, 1.0);
}




// Translation matrix
Matrix4 Matrix4::translate(double dx, double dy, double dz) {
    Matrix4 mat;
    mat.m[0][3] = dx;
    mat.m[1][3] = dy;
    mat.m[2][3] = dz;
    return mat;
}

// Scaling matrix
Matrix4 Matrix4::scale(double sx, double sy, double sz) {
    Matrix4 mat;
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

// Rotation matrix around X-axis
Matrix4 Matrix4::rotateX(double angle) {
    Matrix4 mat;
    double cosA = cos(angle), sinA = sin(angle);
    mat.m[1][1] = cosA;
    mat.m[1][2] = -sinA;
    mat.m[2][1] = sinA;
    mat.m[2][2] = cosA;
    return mat;
}

// Rotation matrix around Y-axis
Matrix4 Matrix4::rotateY(double angle) {
    Matrix4 mat;
    double cosA = cos(angle), sinA = sin(angle);
    mat.m[0][0] = cosA;
    mat.m[0][2] = sinA;
    mat.m[2][0] = -sinA;
    mat.m[2][2] = cosA;
    return mat;
}

// Rotation matrix around Z-axis
Matrix4 Matrix4::rotateZ(double angle) {
    Matrix4 mat;
    double cosA = cos(angle), sinA = sin(angle);
    mat.m[0][0] = cosA;
    mat.m[0][1] = -sinA;
    mat.m[1][0] = sinA;
    mat.m[1][1] = cosA;
    return mat;
}

// Orthographic projection matrix
Matrix4 Matrix4::orthographic(double left, double right, double bottom, double top, double near, double far) {
    Matrix4 mat;
    mat.m[0][0] = 2.0 / (right - left);
    mat.m[1][1] = 2.0 / (top - bottom);
    mat.m[2][2] = -2.0 / (far - near);
    mat.m[0][3] = -(right + left) / (right - left);
    mat.m[1][3] = -(top + bottom) / (top - bottom);
    mat.m[2][3] = -(far + near) / (far - near);
    return mat;
}

Matrix4 Matrix4::withoutTranslation() const {
    Matrix4 result = *this; // Start with a copy of the current matrix

    // Zero out the translation components in the 4th column
    result.m[0][3] = 0.0;
    result.m[1][3] = 0.0;
    result.m[2][3] = 0.0;

    return result;
}

// Perspective projection matrix
Matrix4 Matrix4::perspective(double fov, double aspect, double near, double far, double d) {
    Matrix4 mat;
    double tanHalfFOV = tan(fov / 2.0);
    mat.m[0][0] = 1.0 / (aspect * tanHalfFOV);
    mat.m[1][1] = 1.0 / tanHalfFOV;
    mat.m[2][2] = -(far + near) / (far - near);
    mat.m[2][3] = -(2.0 * far * near) / (far - near);
    mat.m[3][2] = -1.0;
    mat.m[3][3] = d; // Modify perspective ratio using d
    return mat;
}

// Modify perspective parameter 'd'
void Matrix4::setPerspectiveD(double d) {
    m[3][3] = d;
}


// Print the matrix (for debugging)
void Matrix4::print() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << m[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Transpose the matrix
Matrix4 Matrix4::transpose() const {
    Matrix4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m[j][i];
        }
    }
    return result;
}


Matrix4 Matrix4::inverse() const {
    Matrix4 result;
    double inv[16], det;
    double mat[16];

    // Flatten the matrix into a 1D array
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat[i * 4 + j] = m[i][j];
        }
    }

    // Compute cofactors and determinant
    inv[0] = mat[5] * mat[10] * mat[15] - mat[5] * mat[11] * mat[14] -
        mat[9] * mat[6] * mat[15] + mat[9] * mat[7] * mat[14] +
        mat[13] * mat[6] * mat[11] - mat[13] * mat[7] * mat[10];

    inv[4] = -mat[4] * mat[10] * mat[15] + mat[4] * mat[11] * mat[14] +
        mat[8] * mat[6] * mat[15] - mat[8] * mat[7] * mat[14] -
        mat[12] * mat[6] * mat[11] + mat[12] * mat[7] * mat[10];

    // Determinant calculation
    det = mat[0] * inv[0] + mat[1] * inv[4];
    if (std::abs(det) < 1e-6) {
        throw std::runtime_error("Matrix is singular and cannot be inverted.");
    }

    det = 1.0 / det;

    // Scale by determinant
    for (int i = 0; i < 16; ++i) {
        inv[i] *= det;
    }

    // Rebuild the 4x4 matrix
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = inv[i * 4 + j];
        }
    }
    return result;
}