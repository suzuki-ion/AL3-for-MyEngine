#pragma once
#include <KashipanEngine.h>
#include <Math/Camera.h>
#include <Objects.h>

class Reticle2D {
public:
    Reticle2D(Engine *kashipanEngine, KashipanEngine::Camera *camera,
        const KashipanEngine::Vector3 &pos3D);

    void SetPos3D(const KashipanEngine::Vector3 &pos3D) {
        pos3D_ = pos3D;
    }
    const KashipanEngine::Vector3 &GetPos3D() {
        return pos3D_;
    }

    void Update();
    void Draw();

private:
    void MovePos();
    void MoveToController();

    // エンジンへのポインタ
    Engine *kashipanEngine_ = nullptr;
    // カメラへのポインタ
    KashipanEngine::Camera *camera_;

    // 照準のスプライト
    std::unique_ptr<KashipanEngine::Sprite> reticle_;
    // 照準の3D上の座標
    KashipanEngine::Vector3 pos3D_;
    // 照準のワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;

    // アンカーポイント
    KashipanEngine::Vector2 anchor_;
    // 画像へのハンドル
    int textureHandle_;
};

