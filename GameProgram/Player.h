#pragma once
#include "Objects.h"

// 前方宣言
class MapChipField;

class Player {
public:
    // キャラクターの当たり判定サイズ
    static inline const float kWidth = 0.8f;
    static inline const float kHeight = 0.8f;

    enum class LRDirection {
        kRight,
        kLeft,
    };

    enum Corner {
        kRightBottom,	// 右下
        kLeftBottom,	// 左下
        kRightTop,		// 右上
        kLeftTop,       // 左上

        kNumCorner		// 要素数
    };

    struct CollisionMapInfo {
        bool isHitUp = false;
        bool isHitGround = false;
        bool isHitWall = false;
        KashipanEngine::Vector3 velocity = { 0.0f, 0.0f, 0.0f };
    };

    Player() = delete;
    /// @brief 初期化
    /// @param model モデル
    /// @param position 初期位置
    Player(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position);

    // 更新
    void Update();

    // 描画
    void Draw();

    // 座標の設定
    void SetPosition(const KashipanEngine::Vector3 &position) { worldTransform_.translate = position; }

    // デバッグカメラ切り替え
    void ToggleDebugCamera() { isDebugCameraActive_ = !isDebugCameraActive_; }

    // ワールドトランスフォームの取得
    KashipanEngine::Transform &GetWorldTransform() { return worldTransform_; }

    // 速度の取得
    const KashipanEngine::Vector3 &GetVelocity() const { return velocity_; }

    /// @brief マップチップフィールドの設定
    /// @param mapChipField マップチップフィールド
    void SetMapChipField(MapChipField *mapChipField) { mapChipField_ = mapChipField; }

private:
    // 移動処理
    void Move();
    // 当たり判定処理
    void CollisionCheck();
    void CollisionCheckUp(CollisionMapInfo &collisionMapInfo);
    void CollisionCheckDown(CollisionMapInfo &collisionMapInfo);
    void CollisionCheckLeft(CollisionMapInfo &collisionMapInfo);
    void CollisionCheckRight(CollisionMapInfo &collisionMapInfo);
    // 判定結果を反映して移動させる
    void ApplyCollisionResult(const CollisionMapInfo &collisionMapInfo);
    // 天井に接している場合の処理
    void OnHitCeiling(const CollisionMapInfo &collisionMapInfo);

    // 旋回処理
    void Turn();

    // ワールド変換データ
    KashipanEngine::Transform worldTransform_;
    // モデル
    KashipanEngine::Model *model_ = nullptr;
    // マップチップフィールド
    MapChipField *mapChipField_ = nullptr;

    // プレイヤーの移動速度
    KashipanEngine::Vector3 velocity_ = {};
    // プレイヤーの移動方向
    LRDirection lrDirection_ = LRDirection::kRight;
    // 旋回開始時の角度
    float turnFirstRotationY_ = 0.0f;
    // 旋回タイマー
    float turnTimer_ = 0.0f;
    // 接地状態フラグ
    bool onGround_ = true;

    // デバッグカメラ有効
    bool isDebugCameraActive_ = false;
};
