#pragma once
#include <KashipanEngine.h>

class PlayerBullet;
class Enemy;

class BasePlayerBulletState {
public:
    BasePlayerBulletState(Engine *kashipanEngine, PlayerBullet *playerBullet, Enemy *targetEnemy) :
        kashipanEngine_(kashipanEngine),
        playerBullet_(playerBullet),
        targetEnemy_(targetEnemy) {
    };
    virtual ~BasePlayerBulletState() = default;

    void CheckTargetEnemyExist();
    virtual void Update() = 0;
protected:
    Engine *kashipanEngine_;
    PlayerBullet *playerBullet_;
    Enemy *targetEnemy_;
};

