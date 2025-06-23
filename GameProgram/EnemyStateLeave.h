#pragma once
#include "BaseEnemyState.h"

class EnemyStateLeave : public BaseEnemyState {
public:
    EnemyStateLeave(Engine *kashipanEngine, Enemy *enemy);
    void Update() override;
private:
    KashipanEngine::Vector3 velocity_;
};

