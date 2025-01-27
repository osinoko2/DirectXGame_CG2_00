#pragma once
#include "Vector4.h"
#include <cstdint>
#include "Matrix4x4.h"

struct Material final{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};