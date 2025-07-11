#include <Base/Renderer.h>

#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"

using namespace KashipanEngine;

namespace {

// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;

Vector3 TransformNormal(const Vector3 &v, const Matrix4x4 &m) {
    // ベクトルを変換するための行列を適用
    Vector3 result;
    result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
    result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
    result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
    return result;
}

}

Enemy::Enemy(Engine *kashipanEngine) {
    sKashipanEngine = kashipanEngine;
    // エンジンのレンダラーを取得
    Renderer *renderer = sKashipanEngine->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Enemy", "enemy.obj");
    model_->SetRenderer(renderer);

    bulletModel_ = std::make_unique<Model>("Resources/Bullet", "bullet.obj");
    bulletModel_->SetRenderer(renderer);

    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_ = { -16.0f, -8.0f, 128.0f };

    // 初期フェーズの設定
    ChangeState(std::make_unique<EnemyStateApproach>(sKashipanEngine, this));

    // 弾の時限発動の設定
    timedCall_ = std::make_unique<TimedCall>(
        std::bind(&Enemy::Fire, this), 1.0f, true
    );
}

void Enemy::AddTranslate(const KashipanEngine::Vector3 &translate) {
    worldTransform_->translate_ += translate;
}

void Enemy::SetBulletFireEnable(bool enable) {
    if (enable) {
        if (timedCall_ != nullptr) {
            return;
        }
        timedCall_ = std::make_unique<TimedCall>(
            std::bind(&Enemy::Fire, this), 1.0f, true
        );

    } else {
        if (timedCall_ == nullptr) {
            return;
        }
        timedCall_.reset();
    }
}

void Enemy::SetPlayerPosition(KashipanEngine::Vector3 playerPosition) {
    playerPosition_ = playerPosition;
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

    // 弾の発射処理
    if (timedCall_ != nullptr) {
        timedCall_->Update();
    }
    // 弾の更新
    for (auto &bullet : bullets_) {
        bullet->Update();
    }
    // 弾の削除処理
    bullets_.remove_if([](const std::unique_ptr<EnemyBullet> &bullet) {
        return !bullet->IsAlive();
        });
}

void Enemy::Draw() {
    model_->Draw(*worldTransform_);
    
    // 弾を描画
    for (auto &bullet : bullets_) {
        bullet->Draw();
    }
}

void Enemy::Fire() {
    Vector3 enemyPosition(
        worldTransform_->worldMatrix_.m[3][0],
        worldTransform_->worldMatrix_.m[3][1],
        worldTransform_->worldMatrix_.m[3][2]
    );
    Vector3 kBulletVelocity(playerPosition_ - enemyPosition);
    kBulletVelocity = kBulletVelocity.Normalize() * 32.0f; // 弾の速度を設定

    bullets_.push_back(
        std::make_unique<EnemyBullet>(
            sKashipanEngine,
            bulletModel_.get(),
            worldTransform_->translate_,
            TransformNormal(kBulletVelocity, worldTransform_->worldMatrix_),
            5.0f
        )
    );
}
