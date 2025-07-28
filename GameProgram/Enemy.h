#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <list>
#include <memory>

#include "Collider.h"
#include "BaseEnemyState.h"
#include "EnemyBullet.h"
#include "TimedCall.h"

class GameScene;

class Enemy : public Collider {
public:
    static inline const float kMoveSpeed = 3.0f;
    static inline const float kFireInterval = 1.0f;

    Enemy(Engine *kashipanEngine, const KashipanEngine::Vector3 &spawnPos, GameScene *gameScene);

    void AddTranslate(const KashipanEngine::Vector3 &translate);

    void SetBulletFireEnable(bool enable);
    void SetPlayerPosition(KashipanEngine::Vector3 playerPosition);

    KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }

    bool IsAlive() {
        return isAlive_;
    }

    // 状態遷移
    void ChangeState(std::unique_ptr<BaseEnemyState> newState);

    // 衝突を検知したら呼び出されるコールバック関数
    void OnCollision() override;

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // 弾発射処理
    void Fire();

    // ゲームシーンへのポインタ
    GameScene *gameScene_ = nullptr;

    // プレイヤーのワールド座標
    KashipanEngine::Vector3 playerPosition_;

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
    // 弾のモデル
    std::unique_ptr<KashipanEngine::Model> bulletModel_;

    // 状態管理
    std::unique_ptr<BaseEnemyState> state_;

    // 時限発動のリスト
    std::unique_ptr<TimedCall> timedCall_;

    // 生存フラグ
    bool isAlive_ = true;
};

