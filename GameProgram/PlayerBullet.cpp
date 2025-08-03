<<<<<<< HEAD
﻿#include "CollisionConfig.h"
=======
#include "CollisionConfig.h"
>>>>>>> chaptor3のマージ
#include "PlayerBullet.h"
#include "PlayerBulletStateNormal.h"
#include "PlayerBulletStateHoming.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
}

PlayerBullet::PlayerBullet(Engine *kashipanEngine, const KashipanEngine::Vector3 &position,
    const KashipanEngine::Vector3 &velocity, float lifeTime, Enemy *enemy) : kLifeTime(lifeTime) {
    if (kashipanEngine == nullptr) {
        throw std::invalid_argument("Engine pointer cannot be null");
    }
    sKashipanEngine = kashipanEngine;

    model_ = std::make_unique<Model>("Resources/Bullet", "bullet.obj");
    model_->SetRenderer(sKashipanEngine->GetRenderer());
    
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_ = position;
    targetEnemy_ = enemy;
    velocity_ = velocity;
    RotateFromVelocity();
    lifeTimeCounter_ = 0.0f;
    isAlive_ = true;

    if (targetEnemy_) {
        ChangeState(std::make_unique<PlayerBulletStateHoming>(kashipanEngine, this, targetEnemy_));
    } else {
        ChangeState(std::make_unique<PlayerBulletStateNormal>(kashipanEngine, this, targetEnemy_));
    }
    SetCollisionAttribute(kCollisionAttributePlayer);
    SetCollisionMask(std::bitset<8>(kCollisionAttributePlayer).flip());
    SetRadius(0.5f);
}

void PlayerBullet::SetVelocity(const KashipanEngine::Vector3 &velocity) {
    velocity_ = velocity;
    RotateFromVelocity();
}

void PlayerBullet::ChangeState(std::unique_ptr<BasePlayerBulletState> newState) {
    state_ = std::move(newState);
}

void PlayerBullet::OnCollision() {
    isAlive_ = false;
}

void PlayerBullet::Update() {
    if (!isAlive_) {
        return;
    }

    state_->Update();
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