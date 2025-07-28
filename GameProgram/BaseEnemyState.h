#pragma once
#include <KashipanEngine.h>

class Enemy;

class BaseEnemyState {
public:
    BaseEnemyState(Engine *kashipanEngine, Enemy *enemy) :
        kashipanEngine_(kashipanEngine),
        enemy_(enemy) {
    };
    virtual ~BaseEnemyState() = default;
    virtual void Update() = 0;
protected:
    Engine *kashipanEngine_;
    Enemy *enemy_;
};

