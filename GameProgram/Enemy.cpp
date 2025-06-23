#include <Base/Renderer.h>

#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"

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

    // 初期フェーズの設定
    ChangeState(std::make_unique<EnemyStateApproach>(sKashipanEngine, this));
}

void Enemy::AddTranslate(const KashipanEngine::Vector3 &translate) {
    worldTransform_->translate_ += translate;
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> newState) {
    state_ = std::move(newState);
}

void Enemy::Update() {
    state_->Update();

    // z座標が0以下になったら状態を変更
    if (worldTransform_->translate_.z < 0.0f) {
        ChangeState(std::make_unique<EnemyStateLeave>(sKashipanEngine, this));
    }
}

void Enemy::Draw() {
    model_->Draw(*worldTransform_);
}
