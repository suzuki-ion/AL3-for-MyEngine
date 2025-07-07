#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <memory>

#include "Collider.h"

class PlayerBullet : public Collider {
public:
    PlayerBullet(Engine *kashipanEngine, KashipanEngine::Model *model,
        const KashipanEngine::Vector3 &position, const KashipanEngine::Vector3 &velocity, float lifeTime);

    bool IsAlive() const { return isAlive_; }
    
    KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
    
    // 衝突を検知したら呼び出されるコールバック関数
    void OnCollision() override;

    void Update();
    void Draw();

private:
    // 弾が消えるまでの時間
    const float kLifeTime;
    // 弾が消えるまでのカウンター
    float lifeTimeCounter_;
    // 生存フラグ
    bool isAlive_;

    // モデルデータ
    KashipanEngine::Model *model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // 速度
    KashipanEngine::Vector3 velocity_;
};