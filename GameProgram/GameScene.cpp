#include "GameScene.h"
#include <Base/Renderer.h>
#include <Base/Input.h>
#include <2d/ImGuiManager.h>

using namespace KashipanEngine;

namespace {
// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;
// レンダラーへのポインタ
Renderer *sRenderer = nullptr;
}

GameScene::GameScene(Engine *engine) {
    // エンジンへのポインタを保存
    sKashipanEngine = engine;
    // レンダラーへのポインタを取得
    sRenderer = sKashipanEngine->GetRenderer();

    // カメラのインスタンスを作成
    camera_ = std::make_unique<Camera>();
    // レンダラーにカメラを設定
    sRenderer->SetCamera(camera_.get());

    // プレイヤーのインスタンスを作成
    player_ = std::make_unique<Player>(sKashipanEngine);

    // ライトの設定
    light_.direction = Vector3(-0.5f, 0.75f, -0.5f);
    light_.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    light_.intensity = 1.0f;
}

void GameScene::Update() {
    player_->Update();

#ifdef _DEBUG
    if (Input::IsKeyTrigger(DIK_F1)) {
        sRenderer->ToggleDebugCamera();
    }
#endif
}

void GameScene::Draw() {
    sRenderer->PreDraw();
#ifdef _DEBUG
    // デバッグ用のフレームレート表示
    ImGuiManager::Begin("Debug");
    ImGui::Text("FPS: %d", sKashipanEngine->GetFPS());
    ImGui::Text("Delta Time: %.3f ms", sKashipanEngine->GetDeltaTime() * 1000.0f);
    ImGui::End();
#endif // _DEBUG
    sRenderer->SetLight(&light_);
    player_->Draw();
    sRenderer->PostDraw();
}