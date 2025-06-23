#pragma once
#include "BaseEnemyState.h"

class EnemyStateApproach : public BaseEnemyState {
public:
    EnemyStateApproach(Engine *kashipanEngine, Enemy *enemy);
    void Update() override;
private:
    KashipanEngine::Vector3 velocity_;
};

