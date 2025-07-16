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

    // グリッド線
    gridLine_ = std::make_unique<GridLine>(GridLineType::XZ, 1.0f, 10000);
    gridLine_->SetRenderer(sRenderer);

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
    EnemyBullet::SetTargetPosition(player_->GetWorldPosition());
    enemy_->SetPlayerPosition(player_->GetWorldPosition());
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

    gridLine_->Draw();
    
    player_->Draw();
    enemy_->Draw();
    
    sRenderer->PostDraw();
}

void GameScene::CheckAllCollisions() {
    std::list<Collider *> colliders;
    colliders.push_back(player_.get());
    colliders.push_back(enemy_.get());
    for (auto &bullet : player_->GetBullets()) {
        colliders.push_back(bullet.get());
    }
    for (auto &bullet : enemy_->GetBullets()) {
        colliders.push_back(bullet.get());
    }

    // リスト内のペアを総当たり
    auto itrA = colliders.begin();
    for (; itrA != colliders.end(); ++itrA) {
        auto itrB = itrA;
        ++itrB; // itrBはitrAの次の要素から開始
        for (; itrB != colliders.end(); ++itrB) {
            if (((*itrA)->GetCollisionAttribute() & (*itrB)->GetCollisionMask()) == 0 ||
                ((*itrB)->GetCollisionAttribute() & (*itrA)->GetCollisionMask()) == 0) {
                continue; // 衝突しない場合はスキップ
            }
            CheckCollisionPair(*itrA, *itrB);
        }
    }

    //// 自機の弾リスト取得
    //const auto &playerBullets = player_->GetBullets();
    //// 敵の弾リスト取得
    //const auto &enemyBullets = enemy_->GetBullets();

    ////--------- 自機と敵弾 ---------//

    //for (const auto &bullet : enemyBullets) {
    //    CheckCollisionPair(player_.get(), bullet.get());
    //}

    ////--------- 自弾と敵機 ---------//

    //for (const auto &bullet : playerBullets) {
    //    CheckCollisionPair(enemy_.get(), bullet.get());
    //}

    ////--------- 自弾と敵弾 ---------//

    //for (const auto &playerBullet : playerBullets) {
    //    for (const auto &enemyBullet : enemyBullets) {
    //        CheckCollisionPair(playerBullet.get(), enemyBullet.get());
    //    }
    //}
}

void GameScene::CheckCollisionPair(Collider *colliderA, Collider *colliderB) {
    // 判定用の球
    Math::Sphere sphereA(colliderA->GetWorldPosition(), colliderA->GetRadius());
    Math::Sphere sphereB(colliderB->GetWorldPosition(), colliderB->GetRadius());

    if (sphereA.IsCollision(sphereB)) {
        colliderA->OnCollision();
        colliderB->OnCollision();
    }
}
