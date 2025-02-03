#pragma once
#include "VertexData.h"
#include "MaterialData.h"
#include <vector>

struct ModelData final
{
	std::vector<VertexData> vertices;
	MaterialData material;
};