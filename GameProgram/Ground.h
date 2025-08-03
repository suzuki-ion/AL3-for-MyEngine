#pragma once
#include <KashipanEngine.h>
#include <Objects.h>

class Ground {
public:
    Ground(Engine *kashipanEngine);

    // 更新
    void Update();

    // 描画
    void Draw();

private:
    // エンジン
    Engine *kashipanEngine_ = nullptr;

    // ワールド変換データ
    std::unique_ptr<KashipanEngine::WorldTransform> worldTransform_;
    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
};
