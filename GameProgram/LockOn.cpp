#define NOMINMAX
#include <numbers>
#include "LockOn.h"
#include "Enemy.h"

using namespace KashipanEngine;

LockOn::LockOn(Engine *kashipanEngine, KashipanEngine::Camera *camera) {
    kashipanEngine_ = kashipanEngine;
    camera_ = camera;

    reticle_ = std::make_unique<Reticle2D>(kashipanEngine_, camera_, Vector3(0.0f));
}

void LockOn::CheckTargetExist() {
    if (!currentTarget_) {
        return;
    }

    if (!currentTarget_->IsAlive()) {
        currentTarget_ = nullptr;
    }
}

void LockOn::Update(std::list<std::unique_ptr<Enemy>> &enemies) {
    if (enemies.empty()) {
        currentTarget_ = nullptr;
        return;
    }

    std::vector<std::pair<Enemy *, float>> candidates;
    for (auto &enemy : enemies) {
        float distance = referencePoint_.Distance(enemy->GetWorldPosition());
        if (distance <= lockOnRange_) {
            candidates.emplace_back(enemy.get(), distance);
        }
    }

    if (candidates.empty()) {
        currentTarget_ = nullptr;
    } else {
        std::sort(candidates.begin(), candidates.end(),
            [](const auto &a, const auto &b) {
                return a.second < b.second;
            });
        currentTarget_ = candidates.front().first;
        reticle_->SetReticleTo3D(currentTarget_->GetWorldPosition());
    }
}

void LockOn::Draw() {
    if (!currentTarget_) {
        return;
    }
    reticle_->Draw();
}