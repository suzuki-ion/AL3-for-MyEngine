#include "PlayerBulletStateNormal.h"
#include "PlayerBullet.h"

using namespace KashipanEngine;

PlayerBulletStateNormal::PlayerBulletStateNormal(Engine *kashipanEngine, PlayerBullet *playerBullet, Enemy *targetEnemy) :
    BasePlayerBulletState(kashipanEngine, playerBullet, targetEnemy) {
    color_ = { 255.0f, 255.0f, 255.0f, 255.0f };
    playerBullet_->SetColor(color_);
}

void PlayerBulletStateNormal::Update() {
}
