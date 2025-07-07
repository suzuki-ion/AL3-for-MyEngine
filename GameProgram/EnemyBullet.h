#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <memory>

class EnemyBullet {
public:
    static void SetTargetPosition(const KashipanEngine::Vector3 &targetPosition);

    EnemyBullet(Engine *kashipanEngine, KashipanEngine::Model *model,
        const KashipanEngine::Vector3 &position, const KashipanEngine::Vector3 &velocity, float lifeTime, float speed);

    bool IsAlive() const { return isAlive_; }

    void Update();
    void Draw();

private:
    // ターゲットの方向を向く
    void RotateFromVelocity();

    // 弾が消えるまでの時間
    const float kLifeTime;
    // 弾が消えるまでのカウンター
    float lifeTimeCounter_;
    // 弾の飛んでいく速度
    float speed_;
    // 生存フラグ
    bool isAlive_;

    // モデルデータ
    KashipanEngine::Model *model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // 速度
    KashipanEngine::Vector3 velocity_;
};

