#pragma once
#include <Math/Camera.h>
#include <Objects/Lines.h>
#include <Objects/WorldTransform.h>
#include <memory>

class RailCameraController {
public:
    RailCameraController(KashipanEngine::Renderer *renderer);
    RailCameraController(KashipanEngine::Camera *camera, KashipanEngine::Renderer *renderer);

    void SetCamera(KashipanEngine::Camera *camera);
    void SetLoop(bool isLoop) {
        isLoop_ = isLoop;
    }
    
    void Update();
    void DebugDraw();

private:
    float GetTime() const;
    void CatmullRomLines(KashipanEngine::Renderer *renderer);

    std::vector<KashipanEngine::Vector3> points_;
    std::unique_ptr<KashipanEngine::Lines> lines_;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    KashipanEngine::Vector3 nextPos_;
    KashipanEngine::Camera *camera_;

    float time_ = 0.0f;
    float moveSpeed = 32.0f;
    bool isLoop_ = true;
};

