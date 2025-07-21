#include "RailCameraController.h"
#include <KashipanEngine.h>

using namespace KashipanEngine;

RailCameraController::RailCameraController() {
    worldTransform_ = std::make_unique<WorldTransform>();
}

RailCameraController::RailCameraController(KashipanEngine::Camera *camera) {
    worldTransform_ = std::make_unique<WorldTransform>();
    camera_ = camera;
}

void RailCameraController::Update() {
    worldTransform_->translate_.z -= Engine::GetDeltaTime() * 10.0f;
    camera_->SetTranslate(worldTransform_->translate_);
}