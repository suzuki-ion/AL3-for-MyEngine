#include "Skydome.h"
#include <cassert>

using namespace KashipanEngine;

Skydome::Skydome(Engine *kashipanEngine) {
    assert(kashipanEngine != nullptr);
    kashipanEngine_ = kashipanEngine;
    // ワールド変換データの生成
    worldTransform_ = std::make_unique<WorldTransform>();

    // エンジンのレンダラーを取得
    Renderer *renderer = kashipanEngine_->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Skydome", "skydome.obj");
    model_->SetRenderer(renderer);
    for (auto &modelData : model_->GetModels()) {
        auto modelState = modelData.GetStatePtr();
        modelState.material->enableLighting = false;
    }
}

void Skydome::Update() {
	worldTransform_->TransferMatrix();
}

void Skydome::Draw() {
	model_->Draw(*worldTransform_);
}