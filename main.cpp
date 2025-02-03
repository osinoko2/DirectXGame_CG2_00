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
#include "TextureManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "Model.h"
#include "ModelCommon.h"

Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	return MathFunction::Add(v1, v2);
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

	// テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxBase);

	SpriteBase* spriteBase = nullptr;
	// スプライト共通部の初期化
	spriteBase = new SpriteBase();
	spriteBase->Initialize(dxBase);

	Object3dCommon* object3dCommon = nullptr;
	// 3Dオブジェクト共通部の初期化
	object3dCommon = new Object3dCommon();
	object3dCommon->Initialize(dxBase);

	ModelCommon* modelCommon = nullptr;
	// モデル共通部の初期化
	modelCommon = new ModelCommon();
	modelCommon->Initialize(dxBase);

	Model* model = new Model();
	model->Initialize(modelCommon);

	Object3d* object3d = new Object3d();
	object3d->Initialize(object3dCommon);
	object3d->SetModel(model);

	//HRESULT hr;
	
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteBase, "resources/uvChecker.png");

	/*std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		if (i % 2 == 0) {
			sprite->Initialize(spriteBase, "resources/monsterBall.png");
		} else {
			sprite->Initialize(spriteBase, "resources/uvChecker.png");
		}

		sprites.push_back(sprite);
	}*/

	// 頂点リソース用のヒープの設定
	//D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	//uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // uploadHeapを使う

	//// 頂点リソースの設定
	//D3D12_RESOURCE_DESC vertexResourceDesc{};

	//// バッファリソース。テクスチャの場合また別の設定をする
	//vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//vertexResourceDesc.Width = sizeof(VertexData) * 1536;// リソースのサイズ。

	//// バッファの場合はこれらは１にする決まり
	//vertexResourceDesc.Height = 1;
	//vertexResourceDesc.DepthOrArraySize = 1;
	//vertexResourceDesc.MipLevels = 1;
	//vertexResourceDesc.SampleDesc.Count = 1;

	//// バッファの場合はこれにする決まり
	//vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//const uint32_t kSubdivision = 16;

	//hr = dxBase->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	//assert(SUCCEEDED(hr));

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
		/*ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
		ImGui::SliderAngle("CameraRotateX", &cameraTransform.rotate.x);
		ImGui::SliderAngle("CameraRotateY", &cameraTransform.rotate.y);
		ImGui::SliderAngle("CameraRotateZ", &cameraTransform.rotate.z);
		ImGui::SliderAngle("SphereRotateX", &transform.rotate.x);
		ImGui::SliderAngle("SphereRotateY", &transform.rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &transform.rotate.z);
		ImGui::Checkbox("useMonsterBall", &useMonsterBall);*/
		/*ImGui::ColorEdit4("L Color", &directionalLightData->color.x);
		ImGui::DragFloat3("L Direction", &directionalLightData->direction.x, 0.01f);
		if (ImGui::IsItemEdited())
		{
			directionalLightData->direction = MathFunction::Normalize(directionalLightData->direction);
		}
		ImGui::DragFloat("L Intensity", &directionalLightData->intensity, 0.01f);*/
		/*ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);*/

		sprite->Update();

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

		object3dCommon->CommonDrawingSettings();

		object3d->Update();

		object3d->Draw();

		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		spriteBase->CommonDrawingSettings();

		/*int i = 0;

		for (Sprite* sprite : sprites) {

			Vector2 size = sprites[i]->GetSize();
			size.x = 50.0f;
			size.y = 50.0f;
			sprites[i]->SetSize(size);

			Vector2 position = sprites[i]->GetPosition();
			position.x = i * 256.0f;
			position.y = 0;
			sprites[i]->SetPosition(position);

			sprites[i]->Update();
			spriteBase->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(3, object3d->GetDirectionalLightResource()->GetGPUVirtualAddress());
			sprites[i]->Draw();
			++i;
		}*/

		sprite->Draw();

		// 実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxBase->GetCommandList());

		dxBase->PostDraw();
	}

	// ImGuiの終了処理。
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
/*#ifdef _DEBUG
	debugController->Release();
#endif*/ 
	

	// WindowsAPIの終了処理
	winApp->Finalize();

	TextureManager::GetInstance()->Finalize();

	// WindowsAPI解放
	delete input;
	delete winApp;
	delete dxBase;
	delete spriteBase;
	delete sprite;
	delete object3dCommon;
	delete object3d;
	delete modelCommon;
	delete model;

	return 0;
}