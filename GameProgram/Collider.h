#pragma once
#include <Math/Vector3.h>

class Collider {
public:
    // 衝突時に呼ばれる関数
    virtual void OnCollision() = 0;

    virtual KashipanEngine::Vector3 GetWorldPosition() = 0;

    float GetRadius() const {
        return radius_;
    }

    void SetRadius(float radius) {
        radius_ = radius;
    }
private:
    float radius_;
};

