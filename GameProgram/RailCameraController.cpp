#include "RailCameraController.h"
#include <KashipanEngine.h>

using namespace KashipanEngine;

RailCameraController::RailCameraController(Renderer *renderer) {
    worldTransform_ = std::make_unique<WorldTransform>();
    camera_ = nullptr;
    CatmullRomLines(renderer);
}

RailCameraController::RailCameraController(Camera *camera, Renderer *renderer) {
    worldTransform_ = std::make_unique<WorldTransform>();
    camera_ = camera;
    CatmullRomLines(renderer);
}

void RailCameraController::Update() {
    worldTransform_->translate_.z -= Engine::GetDeltaTime() * 10.0f;
    camera_->SetTranslate(worldTransform_->translate_);
}

void RailCameraController::DebugDraw() {
    lines_->Draw();
}

void RailCameraController::CatmullRomLines(Renderer *renderer) {
    const int lineCount = 100;
    lines_ = std::make_unique<Lines>(lineCount);
    lines_->SetRenderer(renderer);

    std::vector<Vector3> points = {
        { 0.0f, 0.0f, 0.0f },
        { 10.0f, 10.0f, 0.0f },
        { 10.0f, 15.0f, 0.0f },
        { 20.0f, 15.0f, 0.0f },
        { 20.0f, 0.0f, 0.0f },
        { 30.0f, 0.0f, 0.0f }
    };
    float elapsedT = 1.0f / static_cast<float>(lineCount);
    // 線に適応
    auto linesStatePtr = lines_->GetStatePtr();
    for (size_t i = 0; i <= lineCount; i++) {
        float t = elapsedT * static_cast<float>(i);
        Vector3 pos = Vector3::CatmullRomPosition(points, t);
        linesStatePtr.vertexData[i].pos = Vector4(pos);
    }
}
