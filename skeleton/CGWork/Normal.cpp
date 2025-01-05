#include "Normal.h"
#include <cmath>

// Default Constructor
Normal::Normal() : start(Vector4()), end(Vector4()) {}

// Parameterized Constructor
Normal::Normal(const Vector4& start, const Vector4& end) : start(start), end(end) {}

// Apply a transformation matrix to the normal
void Normal::transform(const Matrix4& matrix) {
    start = applyTransformation(matrix, start);
    end = applyTransformation(matrix, end);
}

// Helper function to apply a transformation matrix to a Vector4
Vector4 Normal::applyTransformation(const Matrix4& matrix, const Vector4& vec) const {
    float tx = matrix.m[0][0] * vec.x + matrix.m[0][1] * vec.y + matrix.m[0][2] * vec.z + matrix.m[0][3] * vec.w;
    float ty = matrix.m[1][0] * vec.x + matrix.m[1][1] * vec.y + matrix.m[1][2] * vec.z + matrix.m[1][3] * vec.w;
    float tz = matrix.m[2][0] * vec.x + matrix.m[2][1] * vec.y + matrix.m[2][2] * vec.z + matrix.m[2][3] * vec.w;
    float tw = matrix.m[3][0] * vec.x + matrix.m[3][1] * vec.y + matrix.m[3][2] * vec.z + matrix.m[3][3] * vec.w;
    return Vector4(tx, ty, tz, tw);
}

// Check if the normal is back-facing with respect to a given view direction
bool Normal::isBackFacing(const Vector4& viewDirection) const {
    Vector4 normalVector = Vector4(
        end.x - start.x,
        end.y - start.y,
        end.z - start.z,
        0.0f // Normals are direction vectors, so w=0
    );

    float dotProduct = normalVector.x * viewDirection.x +
        normalVector.y * viewDirection.y +
        normalVector.z * viewDirection.z;

    return dotProduct < 0; // Back-facing if the dot product is negative
}


// Apply a transformation to the normal
void Normal::transform(const Matrix4& transform, const Matrix4& normalTransform) {
    Vector4 direction = end - start;
    Vector4 transformedDirection = normalTransform.transform(direction).normalize() * 16.0;
    start = transform.transform(start);
    end = start + transformedDirection;
}

void Normal::flip() {
    Vector4 direction = start - end; // Reverse the direction vector
    end = start + direction;         // Update the end point
}

Vector4 Normal::getVector()
{
    Vector4 normalVector = Vector4(
        end.x - start.x,
        end.y - start.y,
        end.z - start.z,
        0.0f // Normals are direction vectors, so w=0
    );
    return normalVector;

}