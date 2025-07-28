#define NOMINMAX
#include "RailCameraController.h"
#include "Easings.h"
#include <KashipanEngine.h>
#include <Math/RenderingPipeline.h>

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
    worldTransform_->translate_ = Vector3::CatmullRomPosition(points_, time_, isLoop_);
    float t = GetTime();
    time_ += (t * Engine::GetDeltaTime()) * moveSpeed;
    time_ = std::fmod(time_, 1.0f);
    
    // 次の位置を計算してその方向にカメラを向ける
    nextPos_ = Vector3::CatmullRomPosition(points_, time_, isLoop_);
    Vector3 direction = (worldTransform_->translate_ - nextPos_).Normalize();
    float yaw = atan2f(direction.x, -direction.z);
    float pitch = atan2f(direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));
    worldTransform_->rotate_.x = pitch;
    worldTransform_->rotate_.y = -yaw;
    camera_->SetCamera(worldTransform_->translate_, worldTransform_->rotate_, worldTransform_->scale_);
}

void RailCameraController::DebugDraw() {
    lines_->Draw();
}

float RailCameraController::GetTime() const {
    size_t division = isLoop_ ? points_.size() : points_.size() - 1;
    float areaWidth = 1.0f / static_cast<float>(division);
    size_t index    = static_cast<size_t>(time_ / areaWidth);
    index           = std::min(index, division - 1);

    // インデックス取得関数（ループ対応）
    auto GetIndex = [&](int i) -> size_t {
        if (isLoop_) {
            return (i + points_.size()) % points_.size();
        } else {
            return std::clamp(i, 0, static_cast<int>(points_.size()) - 1);
        }
    };

    size_t index1 = GetIndex(static_cast<int>(index));
    size_t index2 = GetIndex(static_cast<int>(index) + 1);

    const auto &p1 = points_[index1];
    const auto &p2 = points_[index2];

    // p1とp2の距離を計算
    float distance = p1.Distance(p2);

    // 距離からtを計算
    float t = areaWidth / distance;
    return t;
}

void RailCameraController::CatmullRomLines(Renderer *renderer) {
    const int lineCount = 100;
    lines_ = std::make_unique<Lines>(lineCount);
    lines_->SetRenderer(renderer);
    
    points_.push_back(Vector3(0.0f, 0.0f, 0.0f));
    points_.push_back(Vector3(50.0f, 0.0f, 0.0f));
    points_.push_back(Vector3(50.0f, 30.0f, 50.0f));
    points_.push_back(Vector3(-20.0f, 70.0f, 50.0f));
    
    float elapsedT = 1.0f / static_cast<float>(lineCount);
    // 線に適応
    auto linesStatePtr = lines_->GetStatePtr();
    for (size_t i = 0; i <= lineCount; i++) {
        float t = elapsedT * static_cast<float>(i);
        Vector3 pos = Vector3::CatmullRomPosition(points_, t, isLoop_);
        linesStatePtr.vertexData[i].pos = Vector4(pos);
    }
}
