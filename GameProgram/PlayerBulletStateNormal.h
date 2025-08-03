#pragma once
#include <Math/Vector4.h>
#include "BasePlayerBulletState.h"

class PlayerBulletStateNormal : public BasePlayerBulletState {
public:
    PlayerBulletStateNormal(Engine *kashipanEngine, PlayerBullet *playerBullet, Enemy *targetEnemy);
    void Update() override;
private:
    KashipanEngine::Vector4 color_;
};

