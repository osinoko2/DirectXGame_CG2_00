#include <Windows.h>
#include <string>
#include <format>
#include <dxgidebug.h>
#include "externals/DirectXTex/DirectXTex.h"
#include <fstream>
#include "Input.h"
#include "WinApp.h"
#include "DirectXBase.h"
#include "SpriteBase.h"
#include "Sprite.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MathFunction.h"

Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return MathFunction::Add(v1, v2);
}

//struct Material
//{
//	Vector4 color;
//	int32_t enableLighting;
//	float padding[3];
//	Matrix4x4 uvTransform;
//};
//
//struct TransformationMatrix
//{
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};
//
//struct Transform
//{
//	Vector3 scale;
//	Vector3 rotate;
//	Vector3 translate;
//};

struct DirectionalLight
{
	Vector4 color; //!< ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity; //!< 輝度
};

struct MaterialData
{
	std::string textureFilePath;
};

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};



MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納する
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	// 1. 中で必要となる変数の宣言
	ModelData modelData; // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.y *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.x = 1.0f - texcoord.x;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f")
		{
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib")
		{
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	// 4. ModelDataを返す
	return modelData;
}

// Windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* winApp = nullptr;
	// WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Intialize();

	Input* input = nullptr;
	// 入力の初期化
	input = new Input();
	input->Intialize(winApp);

	DirectXBase* dxBase = nullptr;
	// DirectXの初期化
	dxBase = new DirectXBase();
	dxBase->Intialize(winApp);

	SpriteBase* spriteBase = nullptr;
	// スプライト共通部の初期化
	spriteBase = new SpriteBase();
	spriteBase->Initialize(dxBase);

	HRESULT hr;

	

	// モデル読み込み
	ModelData modelData = LoadObjFile("resources", "plane.obj");

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxBase->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxBase->CreateTextureResource(metadata);
	dxBase->UploadTextureData(textureResource.Get(), mipImages);

	// 2枚目のTextureを読んで転送する
	DirectX::ScratchImage mipImages2 = DirectXBase::LoadTexture(/*"resources/monsterBall.png"*/modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxBase->CreateTextureResource(metadata2);
	dxBase->UploadTextureData(textureResource2.Get(), mipImages2);

	

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = dxBase->CreateBufferResource(sizeof(DirectionalLight));

	DirectionalLight* directionalLightData = nullptr;

	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;

	// マテリアル用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = dxBase->CreateBufferResource(sizeof(Material));

	// マテリアルにデータを書き込む
	Material* materialData = nullptr;

	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	// 今回は白を書き込んでみる
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialData->enableLighting = true;

	materialData->uvTransform = MathFunction::MakeIdentity4x4();

	// WVP用のリソースを作る。
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxBase->CreateBufferResource(sizeof(const TransformationMatrix));

	// データを書き込む
	TransformationMatrix* wvpData = nullptr;

	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	// 単位行列を書き込んでおく
	wvpData->WVP = MathFunction::MakeIdentity4x4();
	wvpData->World = MathFunction::MakeIdentity4x4();

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う

	// 頂点リソースの設定
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

	const uint32_t kSubdivision = 16;

	// 頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxBase->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();// リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピー

	hr = dxBase->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	Transform uvTransformSprite{
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
	};

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxBase->GetCPUDescriptorHandle(dxBase->GetSrvDescriptorHeap(), dxBase->GetDescriptorSizeSRV(), 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxBase->GetGPUDescriptorHandle(dxBase->GetSrvDescriptorHeap(), dxBase->GetDescriptorSizeSRV(), 2);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxBase->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxBase->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	// ImGui
	textureSrvHandleCPU.ptr += dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// SRVの生成
	dxBase->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	dxBase->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -15.0f} };

	bool useMonsterBall = true;

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteBase);

	// ウィンドウのxボタンが押されるまでループ

	while (true) {
		// Windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
		ImGui::SliderAngle("CameraRotateX", &cameraTransform.rotate.x);
		ImGui::SliderAngle("CameraRotateY", &cameraTransform.rotate.y);
		ImGui::SliderAngle("CameraRotateZ", &cameraTransform.rotate.z);
		ImGui::SliderAngle("SphereRotateX", &transform.rotate.x);
		ImGui::SliderAngle("SphereRotateY", &transform.rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &transform.rotate.z);
		ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		ImGui::ColorEdit4("L Color", &directionalLightData->color.x);
		ImGui::DragFloat3("L Direction", &directionalLightData->direction.x, 0.01f);
		if (ImGui::IsItemEdited())
		{
			directionalLightData->direction = MathFunction::Normalize(directionalLightData->direction);
		}
		ImGui::DragFloat("L Intensity", &directionalLightData->intensity, 0.01f);
		ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);

		//transform.rotate.y += 0.03f;
		Matrix4x4 worldMatrix = MathFunction::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 cameraMatrix = MathFunction::MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = MathFunction::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MathFunction::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = MathFunction::Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 worldViewProjectionMatrix = MathFunction::Multiply(worldMatrix, viewProjectionMatrix);
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldMatrix;

		sprite->Update();

		Vector2 position = sprite->GetPosition();
		position = position + Vector2{ 0.1f, 0.1f };
		sprite->SetPosition(position);
		
		//Matrix4x4 viewProjectionMatrixSprite = MathFunction::Multiply(viewMatrixSprite, projectionMatrixSprite);
		//Matrix4x4 worldViewProjectionMatrixSprite = MathFunction::Multiply(worldMatrixSprite, viewProjectionMatrixSprite);
		

		//Matrix4x4 uvTransformMatrix = MathFunction::MakeScaleMatrix(uvTransformSprite.scale);
		//uvTransformMatrix = MathFunction::Multiply(uvTransformMatrix, MathFunction::MakeRotateZMatrix(uvTransformSprite.rotate.z));
		//uvTransformMatrix = MathFunction::Multiply(uvTransformMatrix, MathFunction::MakeTranslateMatrix(uvTransformSprite.translate));
		//materialDataSprite->uvTransform = uvTransformMatrix;

		ImGui::ShowDemoWindow();

		// ImGuiの内部コマンドを生成する
		ImGui::Render();

		// 入力の更新
		input->Update();

		if (input->TriggerKey(DIK_0)) // 数字の0キーが押されていたら
		{
			OutputDebugStringA("Hit 0\n");
		}

		dxBase->PreDraw();

		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		spriteBase->CommonDrawingSettings();

		// ParaSignatureを設定。PSOに設定してるけど別途設定が必要
		
		dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);  // VBVを設定

		

		// マテリアルCBufferの場所を設定
		dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

		// wvp用のCBufferの場所を設定
		dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

		// SRVDescriptorTableの先頭を設定。
		dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);

		dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

		// 描画
		//dxBase->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		sprite->Draw();

		// 実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxBase->GetCommandList());

		dxBase->PostDraw();
	}

	// ImGuiの終了処理。
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete sprite;
	vertexResource->Release();
	wvpResource->Release();
	materialResource->Release();
	directionalLightResource->Release();
	/*graphicsPipelineState->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();*/
	textureResource2->Release();
	textureResource->Release();
	/*rootSignature->Release();
	signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}*/
	
/*#ifdef _DEBUG
	debugController->Release();
#endif*/ 
	

	// WindowsAPIの終了処理
	winApp->Finalize();

	// WindowsAPI解放
	delete input;
	delete winApp;
	delete dxBase;
	delete spriteBase;

	return 0;
}