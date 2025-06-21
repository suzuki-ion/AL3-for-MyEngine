#include <Base/Renderer.h>

#include "Enemy.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
}

Enemy::Enemy(Engine *kashipanEngine) {
    sKashipanEngine = kashipanEngine;
    // エンジンのレンダラーを取得
    Renderer *renderer = sKashipanEngine->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Enemy", "enemy.obj");
    model_->SetRenderer(renderer);

    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_.z = 128.0f;

    // 速度の設定
    velocity_.z = -kMoveSpeed;
}

void Enemy::Update() {
    worldTransform_->translate_ += velocity_ * sKashipanEngine->GetDeltaTime();
}

void Enemy::Draw() {
    model_->Draw(*worldTransform_);
}