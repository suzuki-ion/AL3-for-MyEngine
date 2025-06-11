#include <Base/Renderer.h>

#include "Player.h"

using namespace KashipanEngine;

Player::Player(Engine *kashipanEngine) {
    kashipanEngine_ = kashipanEngine;
    // エンジンのレンダラーを取得
    Renderer *renderer = kashipanEngine_->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Player", "player.obj");
    model_->SetRenderer(renderer);

    // モデルの初期位置を設定
    for (auto &model : model_->GetModels()) {
        model.GetStatePtr().transform->translate.z = 16.0f;
    }
}

void Player::Update() {
}

void Player::Draw() {
    // モデルを描画
    model_->Draw();
}