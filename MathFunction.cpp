#include "MathFunction.h"

namespace MathFunction {
	Vector3 Normalize(const Vector3& v)
	{
		Vector3 a;
		float b;
		b = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		a.x = v.x / b;
		a.y = v.y / b;
		a.z = v.z / b;
		return a;
	}

	Matrix4x4 MakeRotateXMatrix(float radian) {
		Matrix4x4 a;
		a.m[0][0] = 1;
		a.m[0][1] = 0;
		a.m[0][2] = 0;
		a.m[0][3] = 0;
		a.m[1][0] = 0;
		a.m[1][1] = std::cos(radian);
		a.m[1][2] = std::sin(radian);
		a.m[1][3] = 0;
		a.m[2][0] = 0;
		a.m[2][1] = -std::sin(radian);
		a.m[2][2] = std::cos(radian);
		a.m[2][3] = 0;
		a.m[3][0] = 0;
		a.m[3][1] = 0;
		a.m[3][2] = 0;
		a.m[3][3] = 1;
		return a;
	}

	Matrix4x4 MakeRotateYMatrix(float radian) {
		Matrix4x4 a;
		a.m[0][0] = std::cos(radian);
		a.m[0][1] = 0;
		a.m[0][2] = -std::sin(radian);
		a.m[0][3] = 0;
		a.m[1][0] = 0;
		a.m[1][1] = 1;
		a.m[1][2] = 0;
		a.m[1][3] = 0;
		a.m[2][0] = std::sin(radian);
		a.m[2][1] = 0;
		a.m[2][2] = std::cos(radian);
		a.m[2][3] = 0;
		a.m[3][0] = 0;
		a.m[3][1] = 0;
		a.m[3][2] = 0;
		a.m[3][3] = 1;
		return a;
	}

	Matrix4x4 MakeRotateZMatrix(float radian) {
		Matrix4x4 a;
		a.m[0][0] = std::cos(radian);
		a.m[0][1] = std::sin(radian);
		a.m[0][2] = 0;
		a.m[0][3] = 0;
		a.m[1][0] = -std::sin(radian);
		a.m[1][1] = std::cos(radian);
		a.m[1][2] = 0;
		a.m[1][3] = 0;
		a.m[2][0] = 0;
		a.m[2][1] = 0;
		a.m[2][2] = 1;
		a.m[2][3] = 0;
		a.m[3][0] = 0;
		a.m[3][1] = 0;
		a.m[3][2] = 0;
		a.m[3][3] = 1;
		return a;
	}

	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
		Matrix4x4 a;
		a.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
		a.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
		a.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
		a.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

		a.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
		a.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
		a.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
		a.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

		a.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
		a.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
		a.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
		a.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

		a.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
		a.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
		a.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
		a.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
		return a;
	}

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
		Matrix4x4 a;
		Matrix4x4 b;

		b = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
		a.m[0][0] = scale.x * b.m[0][0];
		a.m[0][1] = scale.x * b.m[0][1];
		a.m[0][2] = scale.x * b.m[0][2];
		a.m[0][3] = 0;
		a.m[1][0] = scale.y * b.m[1][0];
		a.m[1][1] = scale.y * b.m[1][1];
		a.m[1][2] = scale.y * b.m[1][2];
		a.m[1][3] = 0;
		a.m[2][0] = scale.z * b.m[2][0];
		a.m[2][1] = scale.z * b.m[2][1];
		a.m[2][2] = scale.z * b.m[2][2];
		a.m[2][3] = 0;
		a.m[3][0] = translate.x;
		a.m[3][1] = translate.y;
		a.m[3][2] = translate.z;
		a.m[3][3] = 1;

		return a;
	}

	Matrix4x4 MakeIdentity4x4()
	{
		Matrix4x4 a;
		a.m[0][0] = 1; a.m[0][1] = 0; a.m[0][2] = 0; a.m[0][3] = 0;
		a.m[1][0] = 0; a.m[1][1] = 1; a.m[1][2] = 0; a.m[1][3] = 0;
		a.m[2][0] = 0; a.m[2][1] = 0; a.m[2][2] = 1; a.m[2][3] = 0;
		a.m[3][0] = 0; a.m[3][1] = 0; a.m[3][2] = 0; a.m[3][3] = 1;
		return a;
	}

	Matrix4x4 Inverse(const Matrix4x4& m)
	{
		Matrix4x4 a;
		float detA =
			m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
			m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
			m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -

			m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
			m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
			m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -

			m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
			m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
			m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +

			m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
			m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
			m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +

			m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
			m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
			m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -

			m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
			m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
			m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -

			m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
			m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
			m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +

			m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
			m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
			m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

		a.m[0][0] = 1 / detA * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
		a.m[0][1] = 1 / detA * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
		a.m[0][2] = 1 / detA * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
		a.m[0][3] = 1 / detA * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

		a.m[1][0] = 1 / detA * -(m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][2]);
		a.m[1][1] = 1 / detA * -(-m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][2]);
		a.m[1][2] = 1 / detA * -(m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][2]);
		a.m[1][3] = 1 / detA * -(-m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][2]);

		a.m[2][0] = 1 / detA * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
		a.m[2][1] = 1 / detA * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
		a.m[2][2] = 1 / detA * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
		a.m[2][3] = 1 / detA * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

		a.m[3][0] = 1 / detA * -(m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[1][0] * m.m[2][2] * m.m[3][1]);
		a.m[3][1] = 1 / detA * -(-m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][1] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][2] + m.m[0][0] * m.m[2][2] * m.m[3][1]);
		a.m[3][2] = 1 / detA * -(m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][1] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][2] - m.m[0][0] * m.m[1][2] * m.m[3][1]);
		a.m[3][3] = 1 / detA * -(-m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][1] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][2] + m.m[0][0] * m.m[1][2] * m.m[2][1]);

		return a;
	}

	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
	{
		Matrix4x4 a;
		a.m[0][0] = 1 / aspectRatio * 1 / std::tan(fovY / 2);
		a.m[0][1] = 0;
		a.m[0][2] = 0;
		a.m[0][3] = 0;
		a.m[1][0] = 0;
		a.m[1][1] = 1 / std::tan(fovY / 2);
		a.m[1][2] = 0;
		a.m[1][3] = 0;
		a.m[2][0] = 0;
		a.m[2][1] = 0;
		a.m[2][2] = farClip / (farClip - nearClip);
		a.m[2][3] = 1;
		a.m[3][0] = 0;
		a.m[3][1] = 0;
		a.m[3][2] = -nearClip * farClip / (farClip - nearClip);
		a.m[3][3] = 0;
		return a;
	}

	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
	{
		Matrix4x4 a;
		a.m[0][0] = 2 / (right - left);
		a.m[0][1] = 0;
		a.m[0][2] = 0;
		a.m[0][3] = 0;
		a.m[1][0] = 0;
		a.m[1][1] = 2 / (top - bottom);
		a.m[1][2] = 0;
		a.m[1][3] = 0;
		a.m[2][0] = 0;
		a.m[2][1] = 0;
		a.m[2][2] = 1 / (farClip - nearClip);
		a.m[2][3] = 0;
		a.m[3][0] = (left + right) / (left - right);
		a.m[3][1] = (top + bottom) / (bottom - top);
		a.m[3][2] = nearClip / (nearClip - farClip);
		a.m[3][3] = 1;
		return a;
	}

	Matrix4x4 MakeScaleMatrix(Vector3 scale)
	{
		Matrix4x4 a;
		a.m[0][0] = scale.x;
		a.m[0][1] = 0;
		a.m[0][2] = 0;
		a.m[0][3] = 0;
		a.m[1][0] = 0;
		a.m[1][1] = scale.y;
		a.m[1][2] = 0;
		a.m[1][3] = 0;
		a.m[2][0] = 0;
		a.m[2][1] = 0;
		a.m[2][2] = scale.z;
		a.m[2][3] = 0;
		a.m[3][0] = 0;
		a.m[3][1] = 0;
		a.m[3][2] = 0;
		a.m[3][3] = 1;
		return a;
	}

	Matrix4x4 MakeTranslateMatrix(Vector3 translate)
	{
		Matrix4x4 a;
		a.m[0][0] = 1;
		a.m[0][1] = 0;
		a.m[0][2] = 0;
		a.m[0][3] = 0;
		a.m[1][0] = 0;
		a.m[1][1] = 1;
		a.m[1][2] = 0;
		a.m[1][3] = 0;
		a.m[2][0] = 0;
		a.m[2][1] = 0;
		a.m[2][2] = 1;
		a.m[2][3] = 0;
		a.m[3][0] = translate.x;
		a.m[3][1] = translate.y;
		a.m[3][2] = translate.z;
		a.m[3][3] = 1;
		return a;
	}
}
