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

    std::list<Enemy *> GetTargetEnemies() {
        return currentTargetEnemies_;
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

    std::vector<std::unique_ptr<Reticle2D>> reticles_;
    std::list<Enemy *> currentTargetEnemies_;

    float lockOnRange_ = 50.0f;
    int maxLockOnCount_ = 3;
};

