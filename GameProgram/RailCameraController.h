#pragma once
#include <Math/Camera.h>
#include <Objects/WorldTransform.h>
#include <memory>

class RailCameraController {
public:
    RailCameraController();
    RailCameraController(KashipanEngine::Camera *camera);

    void SetCamera(KashipanEngine::Camera *camera);
    
    void Update();
private:
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    KashipanEngine::Camera *camera_;
};

