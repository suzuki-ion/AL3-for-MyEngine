#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <list>

class Player {
public:
    Player(Engine *kashipanEngine);

    // 更新処理
    void Update();
    // 描画処理
    void Draw();

private:
    // エンジンへのポインタ
    Engine *kashipanEngine_;

    // モデル
    std::unique_ptr<KashipanEngine::Model> model_;
};