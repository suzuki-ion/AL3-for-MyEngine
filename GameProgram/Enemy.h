#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <memory>

#include "BaseEnemyState.h"

class Enemy {
public:
    static inline const float kMoveSpeed = 3.0f;

    Enemy(Engine *kashipanEngine);

    void AddTranslate(const KashipanEngine::Vector3 &translate);

    // 状態遷移
    void ChangeState(std::unique_ptr<BaseEnemyState> newState);

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;

    // 状態管理
    std::unique_ptr<BaseEnemyState> state_;
};

