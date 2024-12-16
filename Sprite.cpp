#include "Sprite.h"
#include "SpriteBase.h"

void Sprite::Initialize(SpriteBase* spBase, std::string textureFilePath)
{
	this->spriteBase = spBase;

	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

	CreateVertexData();

	CreateIndexData();

	CreateMaterialData();

	CreateTransformationMatrixData();
}

void Sprite::Update()
{
	// インデックスリソースにデータを書き込む
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

	// 1枚目の三角形
	vertexData[indexData[0]].position = { 0.0f, 1.0f, 0.0f, 1.0f };// 左下
	vertexData[indexData[0]].texcoord = { 0.0f, 1.0f };
	vertexData[indexData[0]].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[indexData[1]].position = { 0.0f, 0.0f, 0.0f, 1.0f };// 左上
	vertexData[indexData[1]].texcoord = { 0.0f, 0.0f };
	vertexData[indexData[1]].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[indexData[2]].position = { 1.0f, 1.0f, 0.0f, 1.0f };// 右下
	vertexData[indexData[2]].texcoord = { 1.0f, 1.0f };
	vertexData[indexData[2]].normal = { 0.0f, 0.0f, -1.0f };

	// 2枚目の三角形
	vertexData[indexData[3]].position = { 0.0f, 0.0f, 0.0f, 1.0f };// 右上
	vertexData[indexData[3]].texcoord = { 0.0f, 0.0f };
	vertexData[indexData[3]].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[indexData[4]].position = { 1.0f, 0.0f, 0.0f, 1.0f };// 右上
	vertexData[indexData[4]].texcoord = { 1.0f, 0.0f };
	vertexData[indexData[4]].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[indexData[5]].position = { 1.0f, 1.0f, 0.0f, 1.0f };// 右下
	vertexData[indexData[5]].texcoord = { 1.0f, 1.0f };
	vertexData[indexData[5]].normal = { 0.0f, 0.0f, -1.0f };

	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	transform.scale = { size.x,size.y };
	transform.translate = { position.x, position.y, 0.0f };
	transform.rotate = { 0.0f,0.0f,rotation };
	Matrix4x4 worldMatrix = MathFunction::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MathFunction::MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MathFunction::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	transformationMatrixData->WVP = MathFunction::Multiply(worldMatrix, MathFunction::Multiply(viewMatrix, projectionMatrix))/*worldViewProjectionMatrixSprite*/;
	transformationMatrixData->World = worldMatrix;
}

void Sprite::Draw()
{
	// Spriteの描画。
	spriteBase->GetDxBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	spriteBase->GetDxBase()->GetCommandList()->IASetIndexBuffer(&indexBufferView);// IBVを設定

	// マテリアルCBufferの場所を設定
	spriteBase->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	// TransformationMatrixCBufferの場所を設定
	spriteBase->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	spriteBase->GetDxBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex));
	// 描画!
	spriteBase->GetDxBase()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateVertexData()
{
	vertexResource = spriteBase->GetDxBase()->CreateBufferResource(sizeof(VertexData) * 6);
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
}

void Sprite::CreateIndexData()
{
	indexResource = spriteBase->GetDxBase()->CreateBufferResource(sizeof(uint32_t) * 6);

	// リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource.Get()->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
}

void Sprite::CreateMaterialData()
{
	materialResource = spriteBase->GetDxBase()->CreateBufferResource(sizeof(Material));

	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	// 
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = false;
	materialData->uvTransform = MathFunction::MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrixData()
{
	transformationMatrixResource = spriteBase->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));

	transformationMatrixResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MathFunction::MakeIdentity4x4();
	transformationMatrixData->World = MathFunction::MakeIdentity4x4();
}
