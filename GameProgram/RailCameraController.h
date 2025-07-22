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
    
    void Update();
    void DebugDraw();

private:
    void CatmullRomLines(KashipanEngine::Renderer *renderer);

    std::unique_ptr<KashipanEngine::Lines> lines_;
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    KashipanEngine::Camera *camera_;
};

