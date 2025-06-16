#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <list>

class Player {
public:
    static inline const float kMoveSpeed = 0.2f;

    Player(Engine *kashipanEngine);

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // 移動関数
    void Move();

    // エンジンへのポインタ
    Engine *kashipanEngine_;

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;

    // 速度
    KashipanEngine::Vector3 velocity_;
};