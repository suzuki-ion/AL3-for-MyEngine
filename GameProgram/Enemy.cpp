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
    worldTransform_->translate_ = { -16.0f, -8.0f, 32.0f };

    // 速度の設定
    velocity_.z = -kMoveSpeed;
}

void Enemy::Update() {
    UpdateApproachPhase();
    UpdateLeavePhase();
}

void Enemy::Draw() {
    model_->Draw(*worldTransform_);
}

void Enemy::UpdateApproachPhase() {
    if (phase_ != Phase::Approach) {
        return;
    }

    worldTransform_->translate_ += velocity_ * sKashipanEngine->GetDeltaTime();
    if (worldTransform_->translate_.z < 0.0f) {
        phase_ = Phase::Leave;
        velocity_.x = kMoveSpeed;
        velocity_.y = kMoveSpeed;
        velocity_.z = 0.0f;
    }
}

void Enemy::UpdateLeavePhase() {
    if (phase_ != Phase::Leave) {
        return;
    }

    worldTransform_->translate_ += velocity_ * sKashipanEngine->GetDeltaTime();
}
