#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include <fstream>
#include <sstream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

struct Vector4
{
	float x, y, z, w;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct DirectionalLight
{
	Vector4 color; //!< ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity; //!< 輝度
};

struct MaterialData
{
	std::string textureFilePath;
};

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

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

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタヒープの生成
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

IDxcBlob* CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,

	// Compilerに使用するProfile
	const wchar_t* profile,

	// 初期化で生成したもの３つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler
) {
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));

	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	assert(SUCCEEDED(hr));

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	LPCWSTR arguments[] =
	{
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E",L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile, // ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od",    // 最適化しておく
		L"-Zpr",   // メモリレイアウトは最優先
	};

	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);

	// コンパイルエラーでなくdxcが起動出来ないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 警告・エラーが出たらログを出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());

		// 警告・エラー
		assert(false);
	}

	// コンパイル結果から実行のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));

	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();

	// 実行用のバイナリを返却
	return shaderBlob;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};

	// バッファリソース。テクスチャの場合また別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;// リソースのサイズ。

	// バッファの場合はこれらは１にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 実際に頂点リソースを作る
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	// テクスチャファイルを読んでプログラムで扱える
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {

	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// 利用するHeapの設定。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {

	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);

		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// 深層値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破壊された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納する
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	// 1. 中で必要となる変数の宣言
	ModelData modelData; // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.y *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.x = 1.0f - texcoord.x;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f")
		{
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib")
		{
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	// 4. ModelDataを返す
	return modelData;
}

// Windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	CoInitializeEx(0, COINIT_MULTITHREADED);

	// 出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名(なんでも良い)
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc);

	// クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName, L"CG2", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wrc.right - wrc.left, wrc.bottom - wrc.top, nullptr, nullptr, wc.hInstance, nullptr
	);

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	// DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;

	// HRESULTはwindow系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));


	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;

	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));// 取得できないのは一大事

		// ソフトウェアアダプタでなければ採用!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr; // ソフトウェアアダプタの場合は見なかった事にする
	}

	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	ID3D12Device* device = nullptr;

	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };

	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));

		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");// 初期化完了のログをだす

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		// ヤバイエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		// 解放
		infoQueue->Release();
	}
#endif

	// コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;

	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// スワップチェーンを生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	swapChainDesc.Width = kClientWidth;// 画面の幅
	swapChainDesc.Height = kClientHeight;// 画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
	swapChainDesc.SampleDesc.Count = 1;// マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;// 描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;// モニタにうつしたら、中身を破棄

	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));

	// RTV用のヒープでディスクリプタの数は2。
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false);

	// SRV用のヒープでディスクリプタの数は128。
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// DSV用のヒープでディスクリプタの数は1。
	ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DescriptorSizeを取得しておく
	const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));

	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));

	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));

	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;// 
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//

	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	// まず1つ目を作る。1つ目は最初のところに作る場所をこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);

	// 2つ目のディスクリプタハンドルを得る(自力で)
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 2つ目を作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

	// DepthStencilTextureをウィンドウのサイズで作成
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// DSVHeapの先頭にDSVをつくる
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// 初期値0でFenceを作る
	ID3D12Fence* fence = nullptr;
	uint32_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

	// dxCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしていないが、includeに対応するための設定をしておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// RootParameter作成。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// モデル読み込み
	ModelData modelData = LoadObjFile("resources", "plane.obj");

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	UploadTextureData(textureResource, mipImages);

	// 2枚目のTextureを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture(/*"resources/monsterBall.png"*/modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	UploadTextureData(textureResource2, mipImages2);

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};

	// すべての要素数を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	IDxcBlob* vertexShaderBlob = CompileShader(L"Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	IDxcBlob* pixelShaderBlob = CompileShader(L"Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;

	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	//比較関数はLessEqual。
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature; // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;  // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };   // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジ(形状)のタイプ。
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// どのように画面に色をつけるかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	ID3D12Resource* directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));

	DirectionalLight* directionalLightData = nullptr;

	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;

	// マテリアル用のリソースを作る。
	ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Material));

	// マテリアルにデータを書き込む
	Material* materialData = nullptr;

	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	// 今回は白を書き込んでみる
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialData->enableLighting = true;

	materialData->uvTransform = MakeIdentity4x4();

	// WVP用のリソースを作る。
	ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));

	// データを書き込む
	TransformationMatrix* wvpData = nullptr;

	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	// 単位行列を書き込んでおく
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};

	// バッファリソース。テクスチャの場合また別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(VertexData) * 1536;// リソースのサイズ。

	// バッファの場合はこれらは１にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//ID3D12Resource* vertexResource = nullptr;

	//// 頂点バッファビューを生成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//// リソースの先頭アドレスから
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

	const uint32_t kSubdivision = 16;

	//int vertexNum = kSubdivision * kSubdivision * 6;

	//// 使用するリソースのサイズは頂点6つ分のサイズ 
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexNum;

	//// １頂点あたりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//// 頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;

	//// 書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 頂点リソースを作る
	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();// リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピー

	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	/*ID3D12Resource* indexResource = CreateBufferResource(device, sizeof(uint32_t) * vertexNum);

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();

	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * vertexNum;

	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 3; indexData[4] = 2; indexData[5] = 1;*/

	/*// 経度分割1つ分の角度。
	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);

	// 緯度分割1つ分の角度。
	const float kLatEvery = float(M_PI) / float(kSubdivision);

	// 緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex){
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;// theta

		// 経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;// fai
			float u = float(lonIndex) / float(kSubdivision);
			float v = 1.0f - float(latIndex) / float(kSubdivision);

			// 頂点にデータを入力する。
			/// a
			vertexData[start].position = { cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon), 1.0f };
			vertexData[start].texcoord = { u, v };
			vertexData[start].normal.x = vertexData[start].position.x;
			vertexData[start].normal.y = vertexData[start].position.y;
			vertexData[start].normal.z = vertexData[start].position.z;

			// 残りの5頂点も順番に計算して入力していく
			/// b
			vertexData[start + 1].position = { cos(lat + kLatEvery) * cos(lon), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon), 1.0f };
			vertexData[start + 1].texcoord = { u, v - 1.0f / float(kSubdivision) };
			vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
			vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
			vertexData[start + 1].normal.z = vertexData[start + 1].position.z;

			/// c
			vertexData[start + 2].position = { cos(lat) * cos(lon + kLonEvery), sin(lat), cos(lat) * sin(lon + kLonEvery), 1.0f };
			vertexData[start + 2].texcoord = { u + 1.0f / float(kSubdivision), v };
			vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
			vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
			vertexData[start + 2].normal.z = vertexData[start + 2].position.z;

			/// d
			vertexData[start + 3].position = {cos(lat + kLatEvery) * cos(lon + kLonEvery), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery), 1.0f};
			vertexData[start + 3].texcoord = { u + 1.0f / float(kSubdivision), v - 1.0f / float(kSubdivision) };
			vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
			vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
			vertexData[start + 3].normal.z = vertexData[start + 3].position.z;

			vertexData[start + 5].position = { cos(lat + kLatEvery) * cos(lon), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon), 1.0f };
			vertexData[start + 5].texcoord = { u, v - 1.0f / float(kSubdivision) };
			vertexData[start + 5].normal.x = vertexData[start + 5].position.x;
			vertexData[start + 5].normal.y = vertexData[start + 5].position.y;
			vertexData[start + 5].normal.z = vertexData[start + 5].position.z;

			vertexData[start + 4].position = { cos(lat) * cos(lon + kLonEvery), sin(lat), cos(lat) * sin(lon + kLonEvery), 1.0f };
			vertexData[start + 4].texcoord = { u + 1.0f / float(kSubdivision), v };
			vertexData[start + 4].normal.x = vertexData[start + 4].position.x;
			vertexData[start + 4].normal.y = vertexData[start + 4].position.y;
			vertexData[start + 4].normal.z = vertexData[start + 4].position.z;
		}
	}*/

	ID3D12Resource* indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();

	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;

	// インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	// Sprite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 6);

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();

	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;

	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	// 1枚目の三角形
	vertexDataSprite[indexDataSprite[0]].position = { 0.0f, 360.0f, 0.0f, 1.0f };// 左下
	vertexDataSprite[indexDataSprite[0]].texcoord = { 0.0f, 1.0f };
	vertexDataSprite[indexDataSprite[0]].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[indexDataSprite[1]].position = { 0.0f, 0.0f, 0.0f, 1.0f };// 左上
	vertexDataSprite[indexDataSprite[1]].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[indexDataSprite[1]].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[indexDataSprite[2]].position = { 640.0f, 360.0f, 0.0f, 1.0f };// 右下
	vertexDataSprite[indexDataSprite[2]].texcoord = { 1.0f, 1.0f };
	vertexDataSprite[indexDataSprite[2]].normal = { 0.0f, 0.0f, -1.0f };

	// 2枚目の三角形
	vertexDataSprite[indexDataSprite[3]].position = { 0.0f, 0.0f, 0.0f, 1.0f };// 左上
	vertexDataSprite[indexDataSprite[3]].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[indexDataSprite[3]].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[indexDataSprite[4]].position = { 640.0f, 0.0f, 0.0f, 1.0f };// 右上
	vertexDataSprite[indexDataSprite[4]].texcoord = { 1.0f, 0.0f };
	vertexDataSprite[indexDataSprite[4]].normal = { 0.0f, 0.0f, -1.0f };
	vertexDataSprite[indexDataSprite[5]].position = { 640.0f, 360.0f, 0.0f, 1.0f };// 右下
	vertexDataSprite[indexDataSprite[5]].texcoord = { 1.0f, 1.0f };
	vertexDataSprite[indexDataSprite[5]].normal = { 0.0f, 0.0f, -1.0f };

	// Sprite用のマテリアルリソースを作る。
	ID3D12Resource* materialResourceSprite = CreateBufferResource(device, sizeof(Material));

	// マテリアルにデータを書き込む
	Material* materialDataSprite = nullptr;

	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

	// 今回は白を書き込んでみる
	materialDataSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	// SpriteはLightingしないのでfalseを設定する
	materialDataSprite->enableLighting = false;

	materialDataSprite->uvTransform = MakeIdentity4x4();

	// Sprite用のTransformationMatrix用のリソースを作る。
	ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));

	// データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;

	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));

	// 単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();

	Transform transformSprite{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	Transform uvTransformSprite{
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
	};

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	// ImGui
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// SRVの生成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

	// ビューポート
	D3D12_VIEWPORT viewport{};

	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シーザー矩形
	D3D12_RECT scissorRect{};

	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;

	// ImGuiの初期化。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);

	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -15.0f} };

	bool useMonsterBall = true;

	// ウィンドウのxボタンが押されるまでループ
	MSG msg{};

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
			ImGui::SliderAngle("CameraRotateX", &cameraTransform.rotate.x);
			ImGui::SliderAngle("CameraRotateY", &cameraTransform.rotate.y);
			ImGui::SliderAngle("CameraRotateZ", &cameraTransform.rotate.z);
			ImGui::SliderAngle("SphereRotateX", &transform.rotate.x);
			ImGui::SliderAngle("SphereRotateY", &transform.rotate.y);
			ImGui::SliderAngle("SphereRotateZ", &transform.rotate.z);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::ColorEdit4("L Color", &directionalLightData->color.x);
			ImGui::DragFloat3("L Direction", &directionalLightData->direction.x, 0.01f);
			if (ImGui::IsItemEdited())
			{
				directionalLightData->direction = Normalize(directionalLightData->direction);
			}
			ImGui::DragFloat("L Intensity", &directionalLightData->intensity, 0.01f);
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);

			//transform.rotate.y += 0.03f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
			Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldMatrix;

			// Sprite用のWorldViewProjectionMatrixを作る
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
			Matrix4x4 viewProjectionMatrixSprite = Multiply(viewMatrixSprite, projectionMatrixSprite);
			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, viewProjectionMatrixSprite);
			transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			transformationMatrixDataSprite->World = worldMatrixSprite;

			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;

			ImGui::ShowDemoWindow();

			// ImGuiの内部コマンドを生成する
			ImGui::Render();

			// これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			// TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};

			// 今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

			// Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			// バリアを張る対象のリソース。
			barrier.Transition.pResource = swapChainResources[backBufferIndex];

			// 遷移前
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

			// 遷移後
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			// TrasitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);

			// 描画先のRTVとDSVを設定する
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);

			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			// 指定した色で画面全体をクリアする
			float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f }; // 青っぽい色。RGBAの順
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearcolor, 0, nullptr);

			// 描画用のDescriptorHeapの設定
			ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
			commandList->SetDescriptorHeaps(1, descriptorHeaps);

			commandList->RSSetViewports(1, &viewport); // Viewportを設定
			commandList->RSSetScissorRects(1, &scissorRect); // Scissorを設定

			// ParaSignatureを設定。PSOに設定してるけど別途設定が必要
			commandList->SetGraphicsRootSignature(rootSignature);
			commandList->SetPipelineState(graphicsPipelineState);  // PSOを設定
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);  // VBVを設定

			// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			// wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

			// SRVDescriptorTableの先頭を設定。
			commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);

			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			// 描画
			//commandList->DrawInstanced(vertexNum, 1, 0, 0);
			commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			// Spriteの描画。
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			commandList->IASetIndexBuffer(&indexBufferViewSprite);// IBVを設定

			// マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());

			// TransformationMatrixCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());

			// 描画!
			//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			// 実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

			// 今回はRenderTargetからPresentにする
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

			// TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);

			// コマンドリストの内容を確定させる。すべてのコマンドを積んでからcloseすること
			hr = commandList->Close();
			assert(SUCCEEDED(hr));

			// GPUにコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, commandLists);

			// 
			swapChain->Present(1, 0);

			// Fenceの値を更新
			fenceValue++;

			// GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
			commandQueue->Signal(fence, fenceValue);
			if (fence->GetCompletedValue() < fenceValue)
			{
				// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
				fence->SetEventOnCompletion(fenceValue, fenceEvent);

				// イベント待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			// 
			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator, nullptr);
			assert(SUCCEEDED(hr));

			// ウィンドウを表示
			ShowWindow(hwnd, SW_SHOW);
		}
	}

	// ImGuiの終了処理。
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	transformationMatrixResourceSprite->Release();
	materialResourceSprite->Release();
	vertexResourceSprite->Release();
	indexResourceSprite->Release();
	vertexResource->Release();
	wvpResource->Release();
	materialResource->Release();
	directionalLightResource->Release();
	graphicsPipelineState->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
	textureResource2->Release();
	textureResource->Release();
	rootSignature->Release();
	signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}
	includeHandler->Release();
	dxcCompiler->Release();
	dxcUtils->Release();
	CloseHandle(fenceEvent);
	fence->Release();
	depthStencilResource->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	dsvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
	/*dxcCompiler->Release();
	dxcUtils->Release();
	includeHandler->Release();
	indexResourceSprite->Release();
	directionalLightResource->Release();
	materialResourceSprite->Release();
	textureResource2->Release();
	depthStencilResource->Release();
	textureResource->Release();
	transformationMatrixResourceSprite->Release();
	wvpResource->Release();
	materialResource->Release();
	vertexResourceSprite->Release();
	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
	CloseHandle(fenceEvent);
	fence->Release();
	dsvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();*/
#ifdef _DEBUG
	debugController->Release();
#endif 
	CloseWindow(hwnd);
	// リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
	CoUninitialize();

	return 0;
}