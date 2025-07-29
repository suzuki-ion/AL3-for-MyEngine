#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <list>
#include <memory>

#include "Collider.h"
#include "PlayerBullet.h"

class GameScene;

class Player : public Collider {
public:
    // 1秒あたりの移動速度
    static inline const float kMoveSpeedSecond = 6.0f;
    // 1秒あたりの回転速度
    static inline const float kRotateSpeedSecond = 6.0f;

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

    Player(Engine *kashipanEngine, KashipanEngine::Camera *camera);

    void SetGameScene(GameScene *gameScene) {
        gameScene_ = gameScene;
    }
    void SetShootDirection(const KashipanEngine::Vector3 &shootDirection) {
        shootDirection_ = shootDirection;
    }

    KashipanEngine::Vector3 GetWorldPosition() override {
        return {
            worldTransform_->worldMatrix_.m[3][0],
            worldTransform_->worldMatrix_.m[3][1],
            worldTransform_->worldMatrix_.m[3][2]
        };
    }
    KashipanEngine::Vector3 GetLocalPosition() {
        return worldTransform_->translate_;
    }

    // 衝突を検知したら呼び出されるコールバック関数
    void OnCollision() override;

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

    // ゲームシーンへのポインタ
    GameScene *gameScene_ = nullptr;

    // カメラへのポインタ
    KashipanEngine::Camera *camera_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // カメラのワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> cameraWorldTransform_;
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

    // 弾の発射方向
    KashipanEngine::Vector3 shootDirection_;
};