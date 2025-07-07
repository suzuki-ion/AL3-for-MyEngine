#include "EnemyBullet.h"

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
}

EnemyBullet::EnemyBullet(Engine *kashipanEngine, Model *model, const Vector3 &position,
    const Vector3 &velocity, float lifeTime) : kLifeTime(lifeTime) {
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
    // 進行方向に向くように回転を設定
    worldTransform_->rotate_ = {
        atan2f(velocity.y, velocity.x),
        atan2f(velocity.z, sqrtf(velocity.x * velocity.x + velocity.y * velocity.y)),
        0.0f
    };
    velocity_ = velocity;
    lifeTimeCounter_ = 0.0f;
    isAlive_ = true;
}

void EnemyBullet::Update() {
    if (!isAlive_) {
        return;
    }

    worldTransform_->translate_ += velocity_ * sKashipanEngine->GetDeltaTime();

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
