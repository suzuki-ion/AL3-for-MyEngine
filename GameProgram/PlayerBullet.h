#pragma once
#include <Objects.h>
#include <memory>

class PlayerBullet {
public:
    PlayerBullet(KashipanEngine::Model *model);

    bool IsShot() const {
        return isShot_;
    }

    void Shot(const KashipanEngine::Vector3 &position);

    void Update();
    void Draw();

private:
    // 発射フラグ
    bool isShot_ = false;

    // モデルデータ
    KashipanEngine::Model *model_;
    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
};