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

void Reticle2D::SetReticleTo2D(const KashipanEngine::Vector2 &pos2D) {
    pos2D_ = pos2D;
    worldTransform_->translate_ = KashipanEngine::Vector3(pos2D_);
    worldTransform_->translate_.x = pos2D_.x - anchor_.x;
    worldTransform_->translate_.y = pos2D_.y - anchor_.y;
}

void Reticle2D::SetReticleTo3D(const KashipanEngine::Vector3 &pos3D) {
    pos3D_ = pos3D;

    Vector3 clipSpacePos = pos3D_.Transform(camera_->GetViewMatrix() * camera_->GetProjectionMatrix());
    worldTransform_->translate_.x = (clipSpacePos.x + 1.0f) * 0.5f * 1920.0f;
    worldTransform_->translate_.y = (1.0f - clipSpacePos.y) * 0.5f * 1080.0f;

    worldTransform_->translate_.x -= anchor_.x;
    worldTransform_->translate_.y -= anchor_.y;
}

void Reticle2D::Update() {
    MovePos();
}

void Reticle2D::Draw() {
    reticle_->Draw(*worldTransform_);
}

void Reticle2D::MovePos() {
    GetPos2D();

    worldTransform_->translate_.x = pos2D_.x - anchor_.x;
    worldTransform_->translate_.y = pos2D_.y - anchor_.y;

    camera_->CalculateMatrix();

    // スクリーン → NDC
    Vector3 ndcNear;
    ndcNear.x = (pos2D_.x / 1920.0f) * 2.0f - 1.0f;
    ndcNear.y = 1.0f - (pos2D_.y / 1080.0f) * 2.0f;
    ndcNear.z = -1.0f;

    Vector3 ndcFar = ndcNear;
    ndcFar.z = 1.0f;

    // 逆投影
    Matrix4x4 invViewProj = (camera_->GetProjectionMatrix() * camera_->GetViewMatrix()).Inverse();
    Vector3 worldNear = ndcNear.Transform(invViewProj);
    Vector3 worldFar = ndcFar.Transform(invViewProj);

    // レイ方向
    Vector3 rayDir = (worldFar - worldNear).Normalize();
    float kDistance = 80.0f;
    pos3D_ = worldNear + rayDir * kDistance;
}

void Reticle2D::GetPos2D() {
    // コントローラーが接続されている場合はコントローラーで位置を更新
    if (Input::IsXBoxConnected(0)) {
        pos2D_.x += Input::GetXBoxRightStickRatioX() * 16.0f;
        pos2D_.y -= Input::GetXBoxRightStickRatioY() * 16.0f;
    } else {
        pos2D_.x = static_cast<float>(Input::GetMouseX());
        pos2D_.y = static_cast<float>(Input::GetMouseY());
    }
}
