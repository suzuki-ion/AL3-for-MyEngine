#pragma once
#include <Math/Vector3.h>
#include <bitset>

class Collider {
public:
    // 衝突時に呼ばれる関数
    virtual void OnCollision() = 0;

    virtual KashipanEngine::Vector3 GetWorldPosition() = 0;

    const std::bitset<8> &GetCollisionAttribute() const {
        return collisionAttribute_;
    }
    const std::bitset<8> &GetCollisionMask() const {
        return collisionMask_;
    }

    float GetRadius() const {
        return radius_;
    }

protected:
    void SetCollisionAttribute(const std::bitset<8> &attribute) {
        collisionAttribute_ = attribute;
    }

    void SetCollisionMask(const std::bitset<8> &mask) {
        collisionMask_ = mask;
    }

    void SetRadius(float radius) {
        radius_ = radius;
    }

private:
    // 衝突属性
    std::bitset<8> collisionAttribute_ = 0b11111111;
    // 衝突マスク
    std::bitset<8> collisionMask_ = 0b11111111;

    float radius_;
};

