#pragma once
#include <Math/Vector4.h>
#include "BasePlayerBulletState.h"

class PlayerBulletStateHoming : public BasePlayerBulletState {
public:
    PlayerBulletStateHoming(Engine *kashipanEngine, PlayerBullet *playerBullet, Enemy *targetEnemy);
    void Update() override;
private:
    KashipanEngine::Vector4 color_;
};

