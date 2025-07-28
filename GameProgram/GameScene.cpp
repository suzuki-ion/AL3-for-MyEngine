#include "GameScene.h"
#include <fstream>
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

    // グリッド線
    gridLine_ = std::make_unique<GridLine>(GridLineType::XZ, 1.0f, 10000);
    gridLine_->SetRenderer(sRenderer);

    // カメラのインスタンスを作成
    camera_ = std::make_unique<Camera>();
    camera_->SetTranslate(Vector3(0.0f, 0.0f, 0.0f));
    camera_->CalculateMatrix();
    // レンダラーにカメラを設定
    sRenderer->SetCamera(camera_.get());

    // カメラコントローラーのインスタンスを作成
    //railCameraController_ = std::make_unique<RailCameraController>(camera_.get(), sRenderer);

    // プレイヤーのインスタンスを作成
    player_ = std::make_unique<Player>(sKashipanEngine, camera_.get());
    player_->SetGameScene(this);
    // 衝突判定管理クラスのインスタンスを作成
    collisionManager_ = std::make_unique<CollisionManager>();
    // スカイドームのインスタンスを作成
    skydome_ = std::make_unique<Skydome>(sKashipanEngine);
    // 地面のインスタンスを作成
    ground_ = std::make_unique<Ground>(sKashipanEngine);

    // ライトの設定
    light_.direction = Vector3(-0.5f, 0.75f, -0.5f);
    light_.color = Vector4(255.0f, 255.0f, 255.0f, 255.0f);
    light_.intensity = 1.0f;

    // 敵発生コマンドの読み込み
    LoadEnemyPopData();
}

void GameScene::Update() {
    player_->Update();
    EnemyBullet::SetTargetPosition(player_->GetWorldPosition());
    for (auto &enemy : enemies_) {
        enemy->SetPlayerPosition(player_->GetWorldPosition());
        enemy->Update();
    }
    enemies_.remove_if([](const std::unique_ptr<Enemy> &enemy) {
        return !enemy->IsAlive();
        });

    // 弾の更新
    for (auto &bullet : playerBullets_) {
        bullet->Update();
    }
    for (auto &bullet : enemyBullets_) {
        bullet->Update();
    }

    // 弾の削除処理
    playerBullets_.remove_if([](const std::unique_ptr<PlayerBullet> &bullet) {
        return !bullet->IsAlive();
        });
    enemyBullets_.remove_if([](const std::unique_ptr<EnemyBullet> &bullet) {
        return !bullet->IsAlive();
        });

    CheckAllCollisions();
    //railCameraController_->Update();
    UpdateEnemyPopCommands();
    

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

    //railCameraController_->DebugDraw();
#endif // _DEBUG
    
    sRenderer->SetLight(&light_);

    gridLine_->Draw();
    skydome_->Draw();
    ground_->Draw();
    
    player_->Draw();
    for (auto &enemy : enemies_) {
        enemy->Draw();
    }
    for (auto &bullet : playerBullets_) {
        bullet->Draw();
    }
    for (auto &bullet : enemyBullets_) {
        bullet->Draw();
    }

    sRenderer->PostDraw();
}

void GameScene::CheckAllCollisions() {
    collisionManager_->ClearColliders();

    collisionManager_->AddCollider(player_.get());
    for (auto &enemy : enemies_) {
        collisionManager_->AddCollider(enemy.get());
    }
    for (auto &bullet : playerBullets_) {
        collisionManager_->AddCollider(bullet.get());
    }
    for (auto &bullet : enemyBullets_) {
        collisionManager_->AddCollider(bullet.get());
    }

    collisionManager_->Update();
}

void GameScene::LoadEnemyPopData() {
    std::ifstream file("Resources/enemy_pop.csv");
    assert(file.is_open());

    // ファイルの内容を文字列ストリームにコピー
    enemyPopCommands_ << file.rdbuf();

    // ファイルを閉じる
    file.close();
}

void GameScene::UpdateEnemyPopCommands() {
    // 待機処理
    if (isEnemyPopWait_) {
        enemyPopWaitTimer_--;
        if (enemyPopWaitTimer_ <= 0) {
            isEnemyPopWait_ = false;
        }
        return;
    }

    // 1行分の文字列を入れる変数
    std::string line;

    // コマンド実行ループ
    while (std::getline(enemyPopCommands_, line)) {
        // 1行分の文字列をストリームに変換して解析しやすくする
        std::istringstream lineStream(line);

        std::string word;
        std::getline(lineStream, word, ',');

        // "//"から始まる行はコメント
        if (word.find("//") == 0) {
            continue;

        } else if (word.find("POP") == 0) {
            //--------- POP ---------//
            
            Vector3 startPos;
            Vector3 endPos;

            // x座標
            std::getline(lineStream, word, ',');
            startPos.x = static_cast<float>(std::atof(word.c_str()));
            // y座標
            std::getline(lineStream, word, ',');
            startPos.y = static_cast<float>(std::atof(word.c_str()));
            // z座標
            std::getline(lineStream, word, ',');
            startPos.z = static_cast<float>(std::atof(word.c_str()));

            // x座標
            std::getline(lineStream, word, ',');
            endPos.x = static_cast<float>(std::atof(word.c_str()));
            // y座標
            std::getline(lineStream, word, ',');
            endPos.y = static_cast<float>(std::atof(word.c_str()));
            // z座標
            std::getline(lineStream, word, ',');
            endPos.z = static_cast<float>(std::atof(word.c_str()));

            // 使うイージング
            std::getline(lineStream, word, ',');
            int useEasingNum = std::atoi(word.c_str());
            // イージング最大時間
            std::getline(lineStream, word, ',');
            float easeMaxTime = static_cast<float>(std::atof(word.c_str()));

            // 敵を発生させる
            PopEnemy(startPos, endPos, useEasingNum, easeMaxTime);

        } else if (word.find("WAIT") == 0) {
            //--------- WAIT ---------//

            std::getline(lineStream, word, ',');

            // 待ち時間
            int32_t waitTime = std::atoi(word.c_str());

            // 待機開始
            isEnemyPopWait_ = true;
            enemyPopWaitTimer_ = waitTime;

            // コマンドループを抜ける
            break;
        }
    }
}

void GameScene::PopEnemy(const KashipanEngine::Vector3 &startPos, const KashipanEngine::Vector3 &endPos, int useEasingNum, float easeMaxTime) {
    enemies_.push_back(std::make_unique<Enemy>(sKashipanEngine, this, startPos, endPos, useEasingNum, easeMaxTime));
}