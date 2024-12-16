#pragma once
#include <string>
#include "externals/DirectXTex/DirectXTex.h"
#include <wrl.h>
#include "d3d12.h"
#include "DirectXBase.h"

class TextureManager
{
public:
	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	// 終了
	void Finalize();
	// 初期化
	void Initialize(DirectXBase* dxBase);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャファイルのパス</param>
	/// <returns>画像イメージデータ</returns>
	void LoadTexture(const std::string& filePath);

	// SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

private:
	static TextureManager* instance;
	DirectXBase* dxBase_ = nullptr;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	// テクスチャ一枚分のデータ
	struct TextureData{
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	// テクスチャデータ
	std::vector<TextureData> textureDatas;

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;
};

