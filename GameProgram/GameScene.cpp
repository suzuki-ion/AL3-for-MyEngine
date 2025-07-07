#include "GameScene.h"
#include <Base/Renderer.h>
#include <Base/Input.h>
#include <2d/ImGuiManager.h>
#include <Math/MathObjects/Sphere.h>

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
    camera_->SetTranslate(Vector3(0.0f, 0.0f, -64.0f));
    camera_->CalculateMatrix();
    // レンダラーにカメラを設定
    sRenderer->SetCamera(camera_.get());

    // プレイヤーのインスタンスを作成
    player_ = std::make_unique<Player>(sKashipanEngine, camera_.get());
    // 敵のインスタンスを作成
    enemy_ = std::make_unique<Enemy>(sKashipanEngine);

    // ライトの設定
    light_.direction = Vector3(-0.5f, 0.75f, -0.5f);
    light_.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    light_.intensity = 1.0f;
}

void GameScene::Update() {
    player_->Update();
    EnemyBullet::SetTargetPosition(player_->GetPosition());
    enemy_->SetPlayerPosition(player_->GetPosition());
    enemy_->Update();
    CheckAllCollisions();

#ifdef _DEBUG
    if (Input::IsKeyTrigger(DIK_F1)) {
        sRenderer->ToggleDebugCamera();
    }
#endif
}

void GameScene::Draw() {
    static int frameRate = 60;
    sRenderer->PreDraw();

#ifdef _DEBUG
    // デバッグ用ImGui
    ImGuiManager::Begin("Debug");
    ImGui::InputInt("Frame Rate", &frameRate, 1, 240);
    ImGui::Text("FPS: %d", Engine::GetFPS());
    ImGui::Text("Delta Time: %.3f ms", Engine::GetDeltaTime() * 1000.0f);
    ImGui::End();

    sKashipanEngine->SetFrameRate(frameRate);
#endif // _DEBUG
    
    sRenderer->SetLight(&light_);
    
    player_->Draw();
    enemy_->Draw();
    
    sRenderer->PostDraw();
}

void GameScene::CheckAllCollisions() {
    // 判定用の球
    Math::Sphere sphereA(Vector3(0.0f), 1.0f);
    Math::Sphere sphereB(Vector3(0.0f), 1.0f);

    // 自機の弾リスト取得
    const auto &playerBullets = player_->GetBullets();
    // 敵の弾リスト取得
    const auto &enemyBullets = enemy_->GetBullets();

    //--------- 自機と敵弾 ---------//

    sphereA.center = player_->GetPosition();
    for (const auto &bullet : enemyBullets) {
        sphereB.center = bullet->GetPosition();
        if (sphereA.IsCollision(sphereB)) {
            player_->OnCollision();
            bullet->OnCollision();
        }
    }

    //--------- 自弾と敵機 ---------//

    sphereA.center = enemy_->GetPosition();
    for (const auto &bullet : playerBullets) {
        sphereB.center = bullet->GetPosition();
        if (sphereA.IsCollision(sphereB)) {
            enemy_->OnCollision();
            bullet->OnCollision();
        }
    }

    //--------- 自弾と敵弾 ---------//

    sphereA.radius = 0.5f;
    sphereB.radius = 0.5f;
    for (const auto &playerBullet : playerBullets) {
        sphereA.center = playerBullet->GetPosition();
        for (const auto &enemyBullet : enemyBullets) {
            sphereB.center = enemyBullet->GetPosition();
            if (sphereA.IsCollision(sphereB)) {
                playerBullet->OnCollision();
                enemyBullet->OnCollision();
            }
        }
    }
}
