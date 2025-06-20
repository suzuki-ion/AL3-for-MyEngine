#pragma once
#include <Objects.h>
#include <memory>

class PlayerBullet {
public:
    PlayerBullet(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position, const KashipanEngine::Vector3 &velocity, uint32_t lifeTime);

    bool IsAlive() const { return isAlive_; }

    void Update();
    void Draw();

private:
    // 弾が消えるまでの時間
    const uint32_t kLifeTime;
    // 弾が消えるまでのカウンター
    uint32_t lifeCounter_;
    // 生存フラグ
    bool isAlive_;

    // モデルデータ
    KashipanEngine::Model *model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // 速度
    KashipanEngine::Vector3 velocity_;
};