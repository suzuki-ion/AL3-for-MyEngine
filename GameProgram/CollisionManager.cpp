#include <Math/MathObjects/Sphere.h>
#include "CollisionManager.h"

using namespace KashipanEngine;

void CollisionManager::AddCollider(Collider *collider) {
    colliders_.push_back(collider);
}

void CollisionManager::ClearColliders() {
    colliders_.clear();
}

void CollisionManager::Update() {
    // リスト内のペアを総当たり
    auto itrA = colliders_.begin();
    for (; itrA != colliders_.end(); ++itrA) {
        auto itrB = itrA;
        ++itrB; // itrBはitrAの次の要素から開始
        for (; itrB != colliders_.end(); ++itrB) {
            if (((*itrA)->GetCollisionAttribute() & (*itrB)->GetCollisionMask()) == 0 ||
                ((*itrB)->GetCollisionAttribute() & (*itrA)->GetCollisionMask()) == 0) {
                continue; // 衝突しない場合はスキップ
            }
            CheckCollisionPair(*itrA, *itrB);
        }
    }
}

void CollisionManager::CheckCollisionPair(Collider *colliderA, Collider *colliderB) {
    // 判定用の球
    Math::Sphere sphereA(colliderA->GetWorldPosition(), colliderA->GetRadius());
    Math::Sphere sphereB(colliderB->GetWorldPosition(), colliderB->GetRadius());

    if (sphereA.IsCollision(sphereB)) {
        colliderA->OnCollision();
        colliderB->OnCollision();
    }
}