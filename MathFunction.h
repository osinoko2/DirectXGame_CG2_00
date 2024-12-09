#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include <sstream>

namespace MathFunction {
	Vector2 Add(const Vector2& v1, const Vector2& v2);
	
	Vector3 Normalize(const Vector3& v);

	Matrix4x4 MakeRotateXMatrix(float radian);

	Matrix4x4 MakeRotateYMatrix(float radian);

	Matrix4x4 MakeRotateZMatrix(float radian);

	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	Matrix4x4 MakeIdentity4x4();

	Matrix4x4 Inverse(const Matrix4x4& m);

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	Matrix4x4 MakeScaleMatrix(Vector3 scale);

	Matrix4x4 MakeTranslateMatrix(Vector3 translate);
}