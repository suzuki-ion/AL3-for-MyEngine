#include "Ground.h"
#include <cassert>

using namespace KashipanEngine;

Ground::Ground(Engine *kashipanEngine) {
    assert(kashipanEngine != nullptr);
    kashipanEngine_ = kashipanEngine;
    // ワールド変換データの生成
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_.y = -100.0f;

    // エンジンのレンダラーを取得
    Renderer *renderer = kashipanEngine_->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Ground", "ground.obj");
    model_->SetRenderer(renderer);
    for (auto &modelData : model_->GetModels()) {
        auto modelState = modelData.GetStatePtr();
        modelState.material->enableLighting = false;
    }
}

void Ground::Update() {
    worldTransform_->TransferMatrix();
}

void Ground::Draw() {
    model_->Draw(*worldTransform_);
}