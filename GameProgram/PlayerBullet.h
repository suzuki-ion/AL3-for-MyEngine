#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <memory>

#include "Collider.h"
#include "BasePlayerBulletState.h"

class Enemy;

class PlayerBullet : public Collider {
public:
    PlayerBullet(Engine *kashipanEngine, const KashipanEngine::Vector3 &position,
        const KashipanEngine::Vector3 &velocity, float lifeTime, Enemy *enemy);

    bool IsAlive() const { return isAlive_; }

    void CheckTargetEnemyExist() {
        state_->CheckTargetEnemyExist();
    }
    
    KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
    void SetColor(const KashipanEngine::Vector4 &color) {
        for (auto &model : model_->GetModels()) {
            model.GetStatePtr().material->color = color;
        }
    }

    void SetVelocity(const KashipanEngine::Vector3 &velocity);

    // 状態遷移
    void ChangeState(std::unique_ptr<BasePlayerBulletState> newState);
    
    // 衝突を検知したら呼び出されるコールバック関数
    void OnCollision() override;

    void Update();
    void Draw();

private:
    void RotateFromVelocity();
    
    // 弾が消えるまでの時間
    const float kLifeTime;
    // 弾が消えるまでのカウンター
    float lifeTimeCounter_;
    // 生存フラグ
    bool isAlive_;

    // モデルデータ
    std::unique_ptr<KashipanEngine::Model> model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // 速度
    KashipanEngine::Vector3 velocity_;

    // 状態管理
    std::unique_ptr<BasePlayerBulletState> state_;
    // ターゲットの敵へのポインタ
    Enemy *targetEnemy_ = nullptr;
};