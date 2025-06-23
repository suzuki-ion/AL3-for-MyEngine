#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <memory>

class Enemy {
public:
    static inline const float kMoveSpeed = 3.0f;

    enum class Phase {
        Approach,
        Leave,
    };

    Enemy(Engine *kashipanEngine);

    // 近接フェーズ更新関数
    void UpdateApproachPhase();
    // 離脱フェーズ更新関数
    void UpdateLeavePhase();

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // フェーズ更新用関数ポインタテーブル
    static void (Enemy::*phaseUpdateFuncTable[])();

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
    // 速度
    KashipanEngine::Vector3 velocity_;

    // 行動フェーズ
    Phase phase_ = Phase::Approach;
};

