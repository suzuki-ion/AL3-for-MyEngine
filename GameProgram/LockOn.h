#pragma once
#include <Math/Vector2.h>
#include <memory>
#include <list>
#include "Reticle2D.h"

class Enemy;

class LockOn {
public:
    LockOn(Engine *kashipanEngine, KashipanEngine::Camera *camera);

    void SetReferencePoint(const KashipanEngine::Vector3 &point) {
        referencePoint_ = point;
    }

    Enemy *GetTargetEnemy() {
        return currentTarget_;
    }

    void CheckTargetExist();

    void Update(std::list<std::unique_ptr<Enemy>> &enemies);
    void Draw();

private:
    // エンジンへのポインタ
    Engine *kashipanEngine_;
    // カメラへのポインタ
    KashipanEngine::Camera *camera_;

    // どこから距離を求めるかの基準点
    KashipanEngine::Vector3 referencePoint_;

    std::unique_ptr<Reticle2D> reticle_;
    Enemy *currentTarget_ = nullptr;

    float lockOnRange_ = 50.0f;
};

