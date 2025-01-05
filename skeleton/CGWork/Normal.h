#ifndef NORMAL_H
#define NORMAL_H

#include "Vector4.h" // Assuming the Vector4 class is defined in Vector4.h
#include "Matrix4.h" // Assuming the Matrix4 class is defined in Matrix4.h

class Normal {
public:
    Vector4 start;
    Vector4 end;

    // Constructors
    Normal();
    Normal(const Vector4& start, const Vector4& end);

    // Apply a transformation matrix to the normal
    void transform(const Matrix4& matrix);

    // Check if the normal is back-facing with respect to a given view direction
    bool isBackFacing(const Vector4& viewDirection) const;
    void transform(const Matrix4& transform, const Matrix4& normalTransform);
    void flip();
    Vector4 getVector();
private:
    // Helper function to apply a transformation matrix to a Vector4
    Vector4 applyTransformation(const Matrix4& matrix, const Vector4& vec) const;
};

#endif // NORMAL_H