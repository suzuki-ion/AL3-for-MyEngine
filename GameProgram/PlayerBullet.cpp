#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position) {
    if (model == nullptr) {
        throw std::invalid_argument("Model pointer cannot be null");
    }
    model_ = model;
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
    worldTransform_->translate_ = position;
}

void PlayerBullet::Update() {
    
}

void PlayerBullet::Draw() {
    model_->Draw(*worldTransform_);
}
