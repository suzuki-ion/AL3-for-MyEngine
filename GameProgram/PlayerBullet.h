#pragma once
#include <Objects.h>
#include <memory>

class PlayerBullet {
public:
    PlayerBullet(KashipanEngine::Model *model, const KashipanEngine::Vector3 &position);

    void Update();
    void Draw();

private:
    // モデルデータ
    KashipanEngine::Model *model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
};