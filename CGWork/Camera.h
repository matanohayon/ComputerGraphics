#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "Vector4.h"
#include "Matrix4.h"

class Camera {
private:
	Camera();
	Matrix4 cTransform;
	Matrix4 projection;

public:
	void setTransformation(const Matrix4& T);
	void setProjection(const Matrix4& T);
	void LookAt(Vector4& eye, Vector4& at, Vector4& up);
	void Ortho(const Matrix4& T);
	void Perspective(const Matrix4& T);



};

















#endif 