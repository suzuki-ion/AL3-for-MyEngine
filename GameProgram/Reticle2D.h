#pragma once
#include <KashipanEngine.h>
#include <Math/Camera.h>
#include <Objects.h>

class Reticle2D {
public:
    Reticle2D(Engine *kashipanEngine, KashipanEngine::Camera *camera,
        const KashipanEngine::Vector3 &pos3D, const std::string &textureName);

    void SetPos2D(const KashipanEngine::Vector2 &pos2D) {
        pos2D_ = pos2D;
    }
    void SetReticleTo2D(const KashipanEngine::Vector2 &pos2D);
    void SetReticleTo3D(const KashipanEngine::Vector3 &pos3D);

    const KashipanEngine::Vector3 &GetPos3D() {
        return pos3D_;
    }

    void Update();
    void Draw();

private:
    void MovePos();
    void GetPos2D();

    // エンジンへのポインタ
    Engine *kashipanEngine_ = nullptr;
    // カメラへのポインタ
    KashipanEngine::Camera *camera_;

    // 照準のスプライト
    std::unique_ptr<KashipanEngine::Sprite> reticle_;
    // 照準の2D上の座標
    KashipanEngine::Vector2 pos2D_;
    // 照準の3D上の座標
    KashipanEngine::Vector3 pos3D_;
    // 照準のワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;

    // アンカーポイント
    KashipanEngine::Vector2 anchor_;
    // 画像へのハンドル
    int textureHandle_;
};

