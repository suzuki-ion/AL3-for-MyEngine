#include "EnemyStateApproach.h"
#include "Enemy.h"

using namespace KashipanEngine;

EnemyStateApproach::EnemyStateApproach(Engine *kashipanEngine, Enemy *enemy) :
    BaseEnemyState(kashipanEngine, enemy) {
    velocity_ = { 0.0f, 0.0f, -Enemy::kMoveSpeed };
}

void EnemyStateApproach::Update() {
    enemy_->AddTranslate(velocity_ * kashipanEngine_->GetDeltaTime());
}
