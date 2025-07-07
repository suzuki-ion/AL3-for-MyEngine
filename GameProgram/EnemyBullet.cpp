#include "EnemyBullet.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
// ターゲットの位置
Vector3 sTargetPosition = Vector3(0.0f, 0.0f, 0.0f);
}

void EnemyBullet::SetTargetPosition(const KashipanEngine::Vector3 &targetPosition) {
    sTargetPosition = targetPosition;
}

EnemyBullet::EnemyBullet(Engine *kashipanEngine, Model *model, const Vector3 &position,
    const Vector3 &velocity, float lifeTime, float speed) : kLifeTime(lifeTime) {
    if (model == nullptr) {
        throw std::invalid_argument("Model pointer cannot be null");
    }
    if (kashipanEngine == nullptr) {
        throw std::invalid_argument("Engine pointer cannot be null");
    }
    sKashipanEngine = kashipanEngine;
    model_ = model;
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_ = position;
    worldTransform_->scale_ = Vector3(1.0f, 1.0f, 3.0f);
    velocity_ = velocity;
    RotateFromVelocity();
    lifeTimeCounter_ = 0.0f;
    speed_ = speed;
    isAlive_ = true;
}

void EnemyBullet::Update() {
    if (!isAlive_) {
        return;
    }

    Vector3 toTarget = sTargetPosition - worldTransform_->translate_;

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

    model_->Draw(*worldTransform_);
}

void EnemyBullet::RotateFromVelocity() {
    Vector3 direction = velocity_.Normalize();
    float yaw = atan2f(direction.x, -direction.z);
    float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    worldTransform_->rotate_ = Vector3(pitch, -yaw, 0.0f);
}
