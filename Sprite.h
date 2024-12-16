#pragma once
#include <Windows.h>
#include "d3d12.h"
#include "stdint.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MathFunction.h"
#include <wrl.h>
#include "TextureManager.h"

class SpriteBase;

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
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

class Sprite
{
public:
	// 初期化
	void Initialize(SpriteBase* spBase, std::string textureFilePath);
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	void CreateVertexData();

	void CreateIndexData();

	void CreateMaterialData();

	void CreateTransformationMatrixData();

	// getter
	const Vector2& GetPosition() const { return position; }

	const Vector2& GetSize() const { return size; }

	// setter
	void SetPosition(const Vector2& position) { this->position = position; }

	void SetSize(const Vector2& size) { this->size = size; }

private:
	SpriteBase* spriteBase = nullptr;
	Vector2 position = { 0.0f, 0.0f };
	float rotation = 0.0f;
	Vector2 size = { 640.0f,360.0f };
	uint32_t textureIndex = 0;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

};

