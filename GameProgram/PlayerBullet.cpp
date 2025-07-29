#include "CollisionConfig.h"
#include "PlayerBullet.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
}

PlayerBullet::PlayerBullet(Engine *kashipanEngine, KashipanEngine::Model *model, const KashipanEngine::Vector3 &position,
    const KashipanEngine::Vector3 &velocity, float lifeTime) : kLifeTime(lifeTime) {
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
    velocity_ = velocity;
    RotateFromVelocity();
    lifeTimeCounter_ = 0.0f;
    isAlive_ = true;

    SetCollisionAttribute(kCollisionAttributePlayer);
    SetCollisionMask(std::bitset<8>(kCollisionAttributePlayer).flip());
    SetRadius(0.5f);
}

void PlayerBullet::OnCollision() {
    isAlive_ = false;
}

void PlayerBullet::Update() {
    if (!isAlive_) {
        return;
    }

    worldTransform_->translate_ += velocity_ * sKashipanEngine->GetDeltaTime();

    lifeTimeCounter_ += sKashipanEngine->GetDeltaTime();
    if (lifeTimeCounter_ >= kLifeTime) {
        isAlive_ = false;
    }
}

void PlayerBullet::Draw() {
    if (!isAlive_) {
        return;
    }

    model_->Draw(*worldTransform_);
}

void PlayerBullet::RotateFromVelocity() {
    Vector3 direction = -velocity_.Normalize();
    float yaw = atan2f(direction.x, -direction.z);
    float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    worldTransform_->rotate_ = Vector3(pitch, -yaw, 0.0f);
}