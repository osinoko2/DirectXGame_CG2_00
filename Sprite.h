#pragma once
#include <Windows.h>
#include "d3d12.h"
#include "stdint.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MathFunction.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include <wrl.h>
#include "TextureManager.h"

class SpriteBase;

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

	const Vector2& GetAnchorPoint() const { return anchorPoint; }

	const bool& GetIsFlipX_() const { return isFlipX_; }

	const bool& GetIsFlipY_() const { return isFlipY_; }

	const Vector2& GetTextureLeftTop() const { return textureLeftTop; }

	const Vector2& GetTextureSize() const { return textureSize; }

	// setter
	void SetPosition(const Vector2& position) { this->position = position; }

	void SetSize(const Vector2& size) { this->size = size; }

	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }

	void SetIsFlipX_(const bool& isFlipX) { this->isFlipX_ = isFlipX; }

	void SetIsFlipY_(const bool& isFlipY) { this->isFlipY_ = isFlipY; }

	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }

	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

private:
	// テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();

	SpriteBase* spriteBase = nullptr;
	Vector2 position = { 0.0f, 0.0f };
	float rotation = 0.0f;
	Vector2 size = { 640.0f,360.0f };
	Vector2 anchorPoint = { 0.0f, 0.0f };
	uint32_t textureIndex = 0;
	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;

	// テクスチャ左上座標
	Vector2 textureLeftTop = { 0.0f, 0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize = { 100.0f, 100.0f };

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

