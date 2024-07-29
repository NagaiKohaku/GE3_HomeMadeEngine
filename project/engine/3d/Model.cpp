#include "Model.h"
#include "DirectXCommon.h"
#include "ModelCommon.h"
#include "TextureManager.h"

#include "Pipeline.h"

#include "fstream"
#include "sstream"
#include "numbers"

void Model::Initialize(const std::string& directoryPath, const std::string& filename) {

	modelCommon_ = ModelCommon::GetInstance();

	//モデルデータの読み込み
	modelData_ = LoadObjFile(directoryPath,filename);

	//頂点リソースを作成
	vertexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	//マテリアルリソースを作成
	materialResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));

	//リソースの先頭のアドレスを取得する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	//使用するリソースのサイズを設定
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());

	//1頂点当たりのサイズを設定
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得する
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//頂点データの設定
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//マテリアルデータの設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = Pipeline::MakeIdentity4x4();

	//モデルデータのテクスチャ設定
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
}

void Model::InitializeSphere(const std::string& directoryPath, const std::string& filename) {

	modelCommon_ = ModelCommon::GetInstance();

	CreateSphereModel();

	//頂点リソースを作成
	vertexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	//マテリアルリソースを作成
	materialResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));

	//リソースの先頭のアドレスを取得する
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	//使用するリソースのサイズを設定
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());

	//1頂点当たりのサイズを設定
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得する
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//頂点データの設定
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	//マテリアルデータの設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = Pipeline::MakeIdentity4x4();

	modelData_.material.textureFilePath = directoryPath + "/" + filename;

	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
}

void Model::Draw() {

	modelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());

	modelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material.textureFilePath));

	modelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

void Model::CreateSphereModel() {

	//分割数
	const uint32_t kSubDivision = 16;

	//経度1つ分の角度 φ
	const float kLonEvery = static_cast<float>(std::numbers::pi) * 2.0f / float(kSubDivision);

	//緯度の1つ分の角度 θ
	const float kLatEvery = static_cast<float>(std::numbers::pi) / float(kSubDivision);

	//緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubDivision; ++latIndex) {

		float lat = -static_cast<float>(std::numbers::pi) / 2.0f + kLatEvery * latIndex; //θ

		//緯度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubDivision; ++lonIndex) {

			float lon = lonIndex * kLonEvery; //φ
			uint32_t start = (latIndex * kSubDivision + lonIndex) * 6;

			VertexData vertexData[6];

			//基準点a 左下
			vertexData[0].position.x = cosf(lat) * cosf(lon);
			vertexData[0].position.y = sinf(lat);
			vertexData[0].position.z = cosf(lat) * sinf(lon);
			vertexData[0].position.w = 1.0f;
			vertexData[0].texcoord = {
				float(lonIndex) / float(kSubDivision),
				1.0f - float(latIndex) / float(kSubDivision)
			};
			vertexData[0].normal.x = vertexData[0].position.x;
			vertexData[0].normal.y = vertexData[0].position.y;
			vertexData[0].normal.z = vertexData[0].position.z;

			//基準点b 左上
			vertexData[1].position.x = cosf(lat + kLatEvery) * cosf(lon);
			vertexData[1].position.y = sinf(lat + kLatEvery);
			vertexData[1].position.z = cosf(lat + kLatEvery) * sinf(lon);
			vertexData[1].position.w = 1.0f;
			vertexData[1].texcoord = {
				float(lonIndex) / float(kSubDivision),
				1.0f - float(latIndex + 1) / float(kSubDivision)
			};
			vertexData[1].normal.x = vertexData[1].position.x;
			vertexData[1].normal.y = vertexData[1].position.y;
			vertexData[1].normal.z = vertexData[1].position.z;

			//基準点c 右下
			vertexData[2].position.x = cosf(lat) * cosf(lon + kLonEvery);
			vertexData[2].position.y = sinf(lat);
			vertexData[2].position.z = cosf(lat) * sinf(lon + kLonEvery);
			vertexData[2].position.w = 1.0f;
			vertexData[2].texcoord = {
				float(lonIndex + 1) / float(kSubDivision),
				1.0f - float(latIndex) / float(kSubDivision)
			};
			vertexData[2].normal.x = vertexData[2].position.x;
			vertexData[2].normal.y = vertexData[2].position.y;
			vertexData[2].normal.z = vertexData[2].position.z;

			//基準点b 左上
			vertexData[3].position.x = cosf(lat + kLatEvery) * cosf(lon);
			vertexData[3].position.y = sinf(lat + kLatEvery);
			vertexData[3].position.z = cosf(lat + kLatEvery) * sinf(lon);
			vertexData[3].position.w = 1.0f;
			vertexData[3].texcoord = {
				float(lonIndex) / float(kSubDivision),
				1.0f - float(latIndex + 1) / float(kSubDivision)
			};
			vertexData[3].normal.x = vertexData[3].position.x;
			vertexData[3].normal.y = vertexData[3].position.y;
			vertexData[3].normal.z = vertexData[3].position.z;

			//基準点d 右上
			vertexData[4].position.x = cosf(lat + kLatEvery) * cosf(lon + kLonEvery);
			vertexData[4].position.y = sinf(lat + kLatEvery);
			vertexData[4].position.z = cosf(lat + kLatEvery) * sinf(lon + kLonEvery);
			vertexData[4].position.w = 1.0f;
			vertexData[4].texcoord = {
				float(lonIndex + 1) / float(kSubDivision),
				1.0f - float(latIndex + 1) / float(kSubDivision)
			};
			vertexData[4].normal.x = vertexData[4].position.x;
			vertexData[4].normal.y = vertexData[4].position.y;
			vertexData[4].normal.z = vertexData[4].position.z;

			//基準点c 右下
			vertexData[5].position.x = cosf(lat) * cosf(lon + kLonEvery);
			vertexData[5].position.y = sinf(lat);
			vertexData[5].position.z = cosf(lat) * sinf(lon + kLonEvery);
			vertexData[5].position.w = 1.0f;
			vertexData[5].texcoord = {
				float(lonIndex + 1) / float(kSubDivision),
				1.0f - float(latIndex) / float(kSubDivision)
			};
			vertexData[5].normal.x = vertexData[5].position.x;
			vertexData[5].normal.y = vertexData[5].position.y;
			vertexData[5].normal.z = vertexData[5].position.z;

			modelData_.vertices.push_back(vertexData[0]);
			modelData_.vertices.push_back(vertexData[1]);
			modelData_.vertices.push_back(vertexData[2]);
			modelData_.vertices.push_back(vertexData[3]);
			modelData_.vertices.push_back(vertexData[4]);
			modelData_.vertices.push_back(vertexData[5]);
		}
	}

}

Model::ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData; //構築するModelData
	std::vector<Vector4> positions; //位置
	std::vector<Vector3> normals; //法線
	std::vector<Vector2> texcoords; //テクスチャ座標
	std::string line; //ファイルから読んだ1行を格納するもの

	VertexData triangle[3];

	//ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	//ファイルが開けたかの確認
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; //先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				//頂点の要素へのIndexは[位置/UV/法線]で格納されているので、分類してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); //区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData; //構築するMaterialData
	std::string line; //ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
	assert(file.is_open()); //開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}