#include "Reticle2D.h"
#include <Base/Texture.h>
#include <Base/Input.h>

using namespace KashipanEngine;

Reticle2D::Reticle2D(Engine *kashipanEngine, KashipanEngine::Camera *camera,
    const KashipanEngine::Vector3 &pos3D) {

    kashipanEngine_ = kashipanEngine;
    camera_ = camera;
    worldTransform_ = std::make_unique<WorldTransform>();

    textureHandle_ = Texture::Load("Resources/reticle.png");
    reticle_ = std::make_unique<Sprite>(textureHandle_);
    reticle_->SetRenderer(kashipanEngine_->GetRenderer());
    pos3D_ = pos3D;

    auto textureData = Texture::GetTexture(textureHandle_);
    anchor_.x = static_cast<float>(textureData.width) / 2.0f;
    anchor_.y = static_cast<float>(textureData.height) / 2.0f;
}

void Reticle2D::Update() {
    MovePos();

    camera_->CalculateMatrix();

    Vector3 clipSpacePos = pos3D_.Transform(camera_->GetViewMatrix() * camera_->GetProjectionMatrix());
    worldTransform_->translate_.x = (clipSpacePos.x + 1.0f) * 0.5f * 1920.0f;
    worldTransform_->translate_.y = (1.0f - clipSpacePos.y) * 0.5f * 1080.0f;

    worldTransform_->translate_.x -= anchor_.x;
    worldTransform_->translate_.y -= anchor_.y;
}

void Reticle2D::Draw() {
    reticle_->Draw(*worldTransform_);
}

void Reticle2D::MovePos() {
    MoveToController();
}

void Reticle2D::MoveToController() {
    pos3D_.x += Input::GetXBoxRightStickRatioX();
    pos3D_.y += Input::GetXBoxRightStickRatioY();
}