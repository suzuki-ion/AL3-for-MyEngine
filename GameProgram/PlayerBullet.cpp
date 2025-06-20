#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position,
    const KashipanEngine::Vector3 &velocity, uint32_t lifeTime) : kLifeTime(lifeTime) {
    if (model == nullptr) {
        throw std::invalid_argument("Model pointer cannot be null");
    }
    model_ = model;
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
    worldTransform_->translate_ = position;
    velocity_ = velocity;
    lifeCounter_ = 0;
    isAlive_ = true;
}

void PlayerBullet::Update() {
    if (!isAlive_) {
        return;
    }

    worldTransform_->translate_ += velocity_;

    lifeCounter_++;
    if (lifeCounter_ >= kLifeTime) {
        isAlive_ = false;
    }
}

void PlayerBullet::Draw() {
    if (!isAlive_) {
        return;
    }

    model_->Draw(*worldTransform_);
}
