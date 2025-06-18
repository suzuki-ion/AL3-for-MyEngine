#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <list>
#include <memory>

#include "PlayerBullet.h"

class Player {
public:
    static inline const float kMoveSpeed = 0.1f;

    enum class MoveDirectionLR {
        kMoveNone,
        kMoveLeft,
        kMoveRight
    };
    enum class MoveDirectionUD {
        kMoveNone,
        kMoveUp,
        kMoveDown
    };

    Player(Engine *kashipanEngine);

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // 移動入力関数
    void InputMove();
    // 回転入力関数
    void InputRotate();

    // 移動関数
    void Move();
    // 回転関数
    void Rotate();
    // 移動制限用関数
    void LimitPosition();
    // 攻撃関数
    void Attack();

    // 弾発射用関数
    void ShootBullet();

    // エンジンへのポインタ
    Engine *kashipanEngine_;

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // 回転専用ベクトル
    KashipanEngine::Vector3 rotateVector_;

    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
    // 弾のモデル
    std::unique_ptr<KashipanEngine::Model> bulletModel_;

    // 速度
    KashipanEngine::Vector3 velocity_;
    // 移動方向(左右)
    MoveDirectionLR moveDirectionLR_ = MoveDirectionLR::kMoveNone;
    // 移動方向(上下)
    MoveDirectionUD moveDirectionUD_ = MoveDirectionUD::kMoveNone;

    // 弾のリスト
    std::list<std::unique_ptr<PlayerBullet>> bullets_;
};