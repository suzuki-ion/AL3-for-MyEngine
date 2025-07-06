#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <list>
#include <memory>

#include "BaseEnemyState.h"
#include "EnemyBullet.h"
#include "TimedCall.h"

class Enemy {
public:
    static inline const float kMoveSpeed = 3.0f;
    static inline const float kFireInterval = 1.0f;

    Enemy(Engine *kashipanEngine);

    void AddTranslate(const KashipanEngine::Vector3 &translate);
    void SetBulletFireEnable(bool enable);

    // 状態遷移
    void ChangeState(std::unique_ptr<BaseEnemyState> newState);

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // 弾発射処理
    void Fire();

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
    // 弾のモデル
    std::unique_ptr<KashipanEngine::Model> bulletModel_;

    // 状態管理
    std::unique_ptr<BaseEnemyState> state_;

    // 弾のリスト
    std::list<std::unique_ptr<EnemyBullet>> bullets_;
    // 時限発動のリスト
    std::unique_ptr<TimedCall> timedCall_;
};

