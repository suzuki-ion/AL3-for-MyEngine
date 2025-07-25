#include "EnemyBullet.h"

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
}

EnemyBullet::EnemyBullet(Engine *kashipanEngine, KashipanEngine::Model *model, const KashipanEngine::Vector3 &position,
    const KashipanEngine::Vector3 &velocity, float lifeTime) : kLifeTime(lifeTime) {
    if (model == nullptr) {
        throw std::invalid_argument("Model pointer cannot be null");
    }
    if (kashipanEngine == nullptr) {
        throw std::invalid_argument("Engine pointer cannot be null");
    }
    sKashipanEngine = kashipanEngine;
    model_ = model;
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
    worldTransform_->translate_ = position;
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
