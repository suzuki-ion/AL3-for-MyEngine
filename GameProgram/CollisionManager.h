#pragma once
#include <list>
#include "Collider.h"

class CollisionManager {
public:
    void AddCollider(Collider *collider);
    void ClearColliders();
    void Update();

private:
    void CheckCollisionPair(Collider *colliderA, Collider *colliderB);

    std::list<Collider *> colliders_;
};