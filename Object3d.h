#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "Transform.h"
#include "TextureManager.h"

class Object3dCommon;

struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};

struct DirectionalLight
{
	Vector4 color; //!< ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity; //!< 輝度
};

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

class Object3d
{
public: // メンバ関数
	// 初期化
	void Initialize(Object3dCommon* object3dCommon);

	void Update();

	void Draw();

	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	// .objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource.Get(); }

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU; }

private:
	Object3dCommon* object3dCommon_ = nullptr;
	// objファイルのデータ
	ModelData modelData;

	// 頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	VertexData* vertexData = nullptr;
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	// マテリアル用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	// マテリアルにデータを書き込む
	Material* materialData = nullptr;

	// WVP用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = nullptr;
	DirectionalLight* directionalLightData = nullptr;

	Transform transform;
	Transform cameraTransform;

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2;

	bool useMonsterBall = true;
};

