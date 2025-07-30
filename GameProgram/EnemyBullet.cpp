#include "CollisionConfig.h"
#include "EnemyBullet.h"
#include <numbers>

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
// ターゲットの位置
Vector3 sTargetPosition = Vector3(0.0f, 0.0f, 0.0f);
}

void EnemyBullet::Initialize(KashipanEngine::Model *bulletModel) {
    bulletModel_ = bulletModel;
}

void EnemyBullet::SetTargetPosition(const KashipanEngine::Vector3 &targetPosition) {
    sTargetPosition = targetPosition;
}

EnemyBullet::EnemyBullet(Engine *kashipanEngine, const KashipanEngine::Vector3 &position,
    const KashipanEngine::Vector3 &velocity, float lifeTime, float speed) : kLifeTime(lifeTime) {
    if (bulletModel_ == nullptr) {
        throw std::invalid_argument("Model pointer cannot be null");
    }
    if (kashipanEngine == nullptr) {
        throw std::invalid_argument("Engine pointer cannot be null");
    }
    sKashipanEngine = kashipanEngine;
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_ = position;
    worldTransform_->scale_ = Vector3(1.0f, 1.0f, 3.0f);
    velocity_ = velocity;
    RotateFromVelocity();
    lifeTimeCounter_ = 0.0f;
    speed_ = speed;
    isAlive_ = true;

    SetCollisionAttribute(kCollisionAttributeEnemy);
    SetCollisionMask(std::bitset<8>(kCollisionAttributeEnemy).flip());
    SetRadius(0.5f);
}

void EnemyBullet::OnCollision() {
    isAlive_ = false;
}

void EnemyBullet::Update() {
    if (!isAlive_) {
        return;
    }

    // ターゲットへの方向を計算する
    Vector3 toTarget = sTargetPosition - worldTransform_->translate_;

    // ターゲットの方向に向かうベクトルを正規化する
    toTarget = toTarget.Normalize();
    velocity_ = velocity_.Normalize();
    velocity_ = Vector3::Slerp(velocity_, toTarget, 0.1f);
    RotateFromVelocity();

    worldTransform_->translate_ += velocity_ * speed_ * sKashipanEngine->GetDeltaTime();

    lifeTimeCounter_ += sKashipanEngine->GetDeltaTime();
    if (lifeTimeCounter_ >= kLifeTime) {
        isAlive_ = false;
    }
}

void EnemyBullet::Draw() {
    if (!isAlive_) {
        return;
    }

    bulletModel_->Draw(*worldTransform_);
}

void EnemyBullet::RotateFromVelocity() {
    Vector3 direction = -velocity_.Normalize();
    float yaw = atan2f(direction.x, -direction.z);
    float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    worldTransform_->rotate_ = Vector3(pitch, -yaw, 0.0f);
}
