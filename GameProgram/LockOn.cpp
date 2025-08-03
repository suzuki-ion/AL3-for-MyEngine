#define NOMINMAX
#include <numbers>
#include <algorithm>
#include "LockOn.h"
#include "Enemy.h"

using namespace KashipanEngine;

LockOn::LockOn(Engine *kashipanEngine, KashipanEngine::Camera *camera) {
    kashipanEngine_ = kashipanEngine;
    camera_ = camera;

    reticles_.resize(maxLockOnCount_);
    for (auto &reticle : reticles_) {
        reticle = std::make_unique<Reticle2D>(kashipanEngine_, camera_, Vector3(0.0f),
            "Resources/target_reticle.png");
    }
}

void LockOn::CheckTargetExist() {
    currentTargetEnemies_.remove_if([](Enemy *enemy) {
        return enemy == nullptr || !enemy->IsAlive();
        });
}

void LockOn::Update(std::list<std::unique_ptr<Enemy>> &enemies) {
    currentTargetEnemies_.clear();
    if (enemies.empty()) {
        return;
    }

    std::vector<std::pair<Enemy*, float>> candidates;
    for (auto &enemy : enemies) {
        float distance = referencePoint_.Distance(enemy->GetWorldPosition());
        if (distance <= lockOnRange_) {
            candidates.emplace_back(enemy.get(), distance);
        }
    }

    std::sort(candidates.begin(), candidates.end(),
        [](const std::pair<Enemy*, float> &a, const std::pair<Enemy*, float> &b) {
            return a.second < b.second; // 距離が近い順にソート
        });

    for (int i = 0; i < std::min(static_cast<int>(candidates.size()), maxLockOnCount_); ++i) {
        currentTargetEnemies_.push_back(candidates[i].first);
        reticles_[i]->SetReticleTo3D(candidates[i].first->GetWorldPosition());
    }
}

void LockOn::Draw() {
    for (size_t i = 0; i < currentTargetEnemies_.size(); ++i) {
        reticles_[i]->Draw();
    }
}