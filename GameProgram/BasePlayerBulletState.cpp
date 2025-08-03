#include "BasePlayerBulletState.h"
#include "Enemy.h"

void BasePlayerBulletState::CheckTargetEnemyExist() {
    if (!targetEnemy_) {
        return;
    }

    if (!targetEnemy_->IsAlive()) {
        targetEnemy_ = nullptr;
    }
}
