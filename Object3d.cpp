#include "Object3d.h"
#include "Object3dCommon.h"
#include "MathFunction.h"

Matrix4x4 operator* (const Matrix4x4 m1, const Matrix4x4 m2) { return MathFunction::Multiply(m1, m2); }

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	this->object3dCommon_ = object3dCommon;

	directionalLightResource = object3dCommon_->GetDxBase()->CreateBufferResource(sizeof(DirectionalLight));
	directionalLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;

	wvpResource = object3dCommon_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));
	// 書き込むためのアドレスを取得
	wvpResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込んでおく
	wvpData->WVP = MathFunction::MakeIdentity4x4();
	wvpData->World = MathFunction::MakeIdentity4x4();

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う

	//// 頂点リソースの設定
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

	transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f, 0.0f}, {0.0f, 4.0f, -10.0f} };
}

void Object3d::Update()
{
	transform.rotate.y += 0.03f;
	Matrix4x4 worldMatrix = MathFunction::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MathFunction::MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = MathFunction::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MathFunction::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	//Matrix4x4 viewProjectionMatrix = MathFunction::Multiply(viewMatrix, projectionMatrix);
	//Matrix4x4 worldViewProjectionMatrix = MathFunction::Multiply(worldMatrix, viewProjectionMatrix);
	wvpData->WVP = worldMatrix * viewMatrix * projectionMatrix;
	wvpData->World = worldMatrix;
}

void Object3d::Draw()
{
	// wvp用のCBufferの場所を設定
	object3dCommon_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	object3dCommon_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	// 3Dモデルが割り当てられていれば描画する
	if (model)
	{
		model->Draw();
	}
}