#include "ModelManager.h"
#include "ModelCommon.h"

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::Initialize() {

	modelCommon_ = ModelCommon::GetInstance();

}

void ModelManager::LoadModel(const std::string& filePath) {

	//読み込み済みモデルの検索
	if (models_.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}

	//モデルの生成とファイルの読み込み
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize("resources", filePath);

	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair(filePath, std::move(model)));
}

void ModelManager::CreateSphere(const std::string& filePath) {

	//読み込み済みモデルの検索
	if (models_.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}

	//モデルの生成とファイルの読み込み
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->InitializeSphere("resources", filePath);

	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath) {

	//読み込み済みモデルの検索
	if (models_.contains(filePath)) {
		//読み込みモデルを戻り値としてreturn
		return models_.at(filePath).get();
	}

	//ファイル名一致なし
	return nullptr;
}