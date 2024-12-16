#pragma once
#include "DirectXBase.h"

class Object3dCommon
{
public: // メンバ関数
	// 初期化
	void Initialize(DirectXBase* dxBase);

	void CommonDrawingSettings();

	DirectXBase* GetDxBase() const { return dxBase_; }

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの生成
	void GenerateGraphicsPipeline();

	DirectXBase* dxBase_;
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;
};

