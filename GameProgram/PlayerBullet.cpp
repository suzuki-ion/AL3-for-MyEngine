#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(KashipanEngine::Model *model) {
    if (model == nullptr) {
        throw std::invalid_argument("Model pointer cannot be null");
    }
    model_ = model;
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
}

void PlayerBullet::Shot(const KashipanEngine::Vector3 &position) {
    isShot_ = true;
    worldTransform_->translate_ = position;
}

void PlayerBullet::Update() {
    if (!isShot_) {
        return;
    }
}

void PlayerBullet::Draw() {
    if (!isShot_) {
        return;
    }
    model_->Draw(*worldTransform_);
}
