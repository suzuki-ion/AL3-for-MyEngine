#include <Base/Renderer.h>

#include "Easings.h"
#include "GameScene.h"
#include "CollisionConfig.h"
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

} // namespace

Enemy::Enemy(Engine *kashipanEngine, GameScene *gameScene,
    const KashipanEngine::Vector3 &startPos, const KashipanEngine::Vector3 &endPos,
    int useEasingNum, float maxEaseTime) {

    sKashipanEngine = kashipanEngine;
    gameScene_ = gameScene;
    // エンジンのレンダラーを取得
    Renderer *renderer = sKashipanEngine->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Enemy", "enemy.obj");
    model_->SetRenderer(renderer);

    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();

    // 最初の位置と最後の位置と使うイージングの値の設定
    startPos_ = startPos;
    endPos_ = endPos;
    useEasingNum_ = useEasingNum;
    maxEaseTime_ = maxEaseTime;

    // 初期フェーズの設定
    ChangeState(std::make_unique<EnemyStateApproach>(sKashipanEngine, this));

    // 弾の時限発動の設定
    timedCall_ = std::make_unique<TimedCall>(
        std::bind(&Enemy::Fire, this), 2.0f, false
    );

    SetCollisionAttribute(kCollisionAttributeEnemy);
    SetCollisionMask(std::bitset<8>(kCollisionAttributeEnemy).flip());
    SetRadius(1.0f);
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

void Enemy::OnCollision() {
    isAlive_ = false;
}

void Enemy::Update() {
    //state_->Update();
    Move();

    // 座標のイージングが終わったら死なせる
    if (currentEaseTimer_ >= maxEaseTime_) {
        isAlive_ = false;
    }

    // 弾の発射処理
    if (timedCall_ != nullptr) {
        timedCall_->Update();
    }
}

void Enemy::Draw() {
    model_->Draw(*worldTransform_);
}

void Enemy::Fire() {
    Vector3 enemyPosition(
        worldTransform_->worldMatrix_.m[3][0],
        worldTransform_->worldMatrix_.m[3][1],
        worldTransform_->worldMatrix_.m[3][2]
    );
    Vector3 kBulletVelocity(playerPosition_ - enemyPosition);
    kBulletVelocity = kBulletVelocity.Normalize();

    std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>(
        sKashipanEngine,
        worldTransform_->translate_,
        TransformNormal(kBulletVelocity, worldTransform_->worldMatrix_),
        5.0f,
        32.0f
    );

    gameScene_->AddEnemyBullet(
        bullet
    );
}

void Enemy::Move() {
    worldTransform_->translate_.x =
        Ease::Auto(currentEaseTimer_, maxEaseTime_, startPos_.x, endPos_.x, useEasingNum_);
    worldTransform_->translate_.y =
        Ease::Auto(currentEaseTimer_, maxEaseTime_, startPos_.y, endPos_.y, useEasingNum_);
    worldTransform_->translate_.z =
        Ease::Auto(currentEaseTimer_, maxEaseTime_, startPos_.z, endPos_.z, useEasingNum_);

    currentEaseTimer_ += Engine::GetDeltaTime();
}