#pragma once
#include "ModelCommon.h"
#include "VertexData.h"
#include "Material.h"
#include "ModelData.h"

class Model
{
public:
	// 初期化
	void Initialize(ModelCommon* modelCommon);

	void Draw();

	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	// .objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU; }

private:
	ModelCommon* modelCommon_;

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

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2;

	bool useMonsterBall = true;
};

