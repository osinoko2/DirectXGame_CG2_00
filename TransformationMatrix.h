#pragma once
#include "Matrix4x4.h"

struct TransformationMatrix final{
	Matrix4x4 WVP;
	Matrix4x4 World;
};