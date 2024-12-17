#include "Camera.h"

void Camera::setTransformation(const Matrix4& T)
{
	cTransform = T;
}

void Camera::setProjection(const Matrix4& T)
{
	projection = T;
}

void Camera::LookAt(Vector4& eye, Vector4& at, Vector4& up)
{
	Vector4 tmp_n = eye - at;
	Vector4 n = tmp_n.normalize();

	Vector4 tmp_u = up.cross(n);
	Vector4 u = tmp_u.normalize();

	Vector4 tmp_v = n.cross(u);
	Vector4 v = tmp_v.normalize();

	Vector4 t(0, 0, 0, 1);

	Matrix4 c(u, v, n, t);





}
