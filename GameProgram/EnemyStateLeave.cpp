#include "EnemyStateLeave.h"
#include "Enemy.h"

EnemyStateLeave::EnemyStateLeave(Engine *kashipanEngine, Enemy *enemy) :
    BaseEnemyState(kashipanEngine, enemy) {
    enemy->SetBulletFireEnable(false);
    velocity_ = { Enemy::kMoveSpeed, Enemy::kMoveSpeed, 0.0f };
}

void EnemyStateLeave::Update() {
    enemy_->AddTranslate(velocity_ * kashipanEngine_->GetDeltaTime());
}
