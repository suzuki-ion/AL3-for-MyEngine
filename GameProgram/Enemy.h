#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Objects/WorldTransform.h>
#include <memory>

class Enemy {
public:
    static inline const float kMoveSpeed = 3.0f;

    Enemy(Engine *kashipanEngine);

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
    // 速度
    KashipanEngine::Vector3 velocity_;
};

