#include "PlayerBulletStateHoming.h"
#include "PlayerBulletStateNormal.h"
#include "PlayerBullet.h"
#include "Enemy.h"

using namespace KashipanEngine;

PlayerBulletStateHoming::PlayerBulletStateHoming(Engine *kashipanEngine, PlayerBullet *playerBullet, Enemy *targetEnemy) :
    BasePlayerBulletState(kashipanEngine, playerBullet, targetEnemy) {
    color_ = { 255.0f, 0.0f, 0.0f, 255.0f };
    playerBullet_->SetColor(color_);
}

void PlayerBulletStateHoming::Update() {
    if (!targetEnemy_ || !targetEnemy_->IsAlive()) {
        playerBullet_->ChangeState(std::make_unique<PlayerBulletStateNormal>(kashipanEngine_, playerBullet_, targetEnemy_));
        return;
    }

    Vector3 bulletVelocity = targetEnemy_->GetWorldPosition() - playerBullet_->GetWorldPosition();
    bulletVelocity = bulletVelocity.Normalize();
    playerBullet_->SetVelocity(bulletVelocity * 16.0f);
}
