#include <Engine.h>
#include <memory>
#include <random>
#include <imgui.h>

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Drawer.h"
#include "Base/TextureManager.h"
#include "Base/Input.h"

#include "2d/ImGuiManager.h"

#include "Math/Camera.h"
#include "Common/ConvertColor.h"

#include "3d/DirectionalLight.h"
#include "Objects/Sphere.h"
#include "Objects/Tetrahedron.h"
#include "Objects/Plane.h"

using namespace MyEngine;

struct AnimationTetrahedron {
    AnimationTetrahedron() = default;
    AnimationTetrahedron(const AnimationTetrahedron &other) {
        tetrahedron.transform = other.tetrahedron.transform;
        tetrahedron.material = other.tetrahedron.material;
        tetrahedron.camera = other.tetrahedron.camera;
        tetrahedron.useTextureIndex = other.tetrahedron.useTextureIndex;
        tetrahedron.normalType = other.tetrahedron.normalType;
        tetrahedron.fillMode = other.tetrahedron.fillMode;
        animationTransform = other.animationTransform;
    }

    Tetrahedron tetrahedron;
    Transform animationTransform;
    int aliveTimer = 0;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    //==================================================
    // 自作ゲームエンジン
    //==================================================

    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> myGameEngine = std::make_unique<Engine>("MyEngine", 1920, 1080, true);

    // WinAppクラスへのポインタ
    WinApp *winApp = myGameEngine->GetWinApp();
    winApp->SetSizeChangeMode(SizeChangeMode::kNormal);
    // DirectXCommonクラスへのポインタ
    DirectXCommon *dxCommon = myGameEngine->GetDxCommon();
    // 描画用クラスへのポインタ
    Drawer *drawer = myGameEngine->GetDrawer();
    // テクスチャ管理クラスへのポインタ
    TextureManager *textureManager = myGameEngine->GetTextureManager();

    // テクスチャを読み込む
    uint32_t textures[2];
    textures[0] = textureManager->Load("Resources/uvChecker.png");
    textures[1] = textureManager->Load("Resources/monsterBall.png");

    // ブレンドモード
    BlendMode blendMode = kBlendModeNormal;

    //==================================================
    // 乱数
    //==================================================

    std::random_device seedGen;
    std::mt19937 engine(seedGen());
    std::uniform_real_distribution<float> rand(-1.0f, 1.0f);

    //==================================================
    // タイマー
    //==================================================

    // 正四面体が消えるまでの時間
    const int kAliveTime = 600;
    // 正四面体が出現するまでの時間
    const int kWaitTime = 30;
    // 正四面体の出現タイマー
    int spawnTimer = 0;

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera = std::make_unique<Camera>(
        winApp,
        Vector3( 0.0f, 2.0f, -16.0f ),
        Vector3( 0.0f, 0.0f, 0.0f ),
        Vector3( 1.0f, 1.0f, 1.0f )
    );
    // デバッグカメラの有効化フラグ
    bool isUseDebugCamera = false;

    //==================================================
    // 背景の色
    //==================================================

    Vector4 clearColor = { 32.0f, 32.0f, 32.0f, 255.0f };

    //==================================================
    // 平行光源
    //==================================================

    DirectionalLight directionalLight = {
        { 255.0f, 255.0f, 255.0f, 255.0f },
        { 0.5f, -0.75f, 0.5f },
        1.0f
    };

    //==================================================
    // 球体
    //==================================================

    Sphere sphere(8);
    sphere.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 2.0f, 0.0f }
    };
    sphere.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    sphere.material.enableLighting = true;
    // カメラを設定
    sphere.camera = camera.get();
    // テクスチャを設定
    sphere.useTextureIndex = -1;
    // 法線の種類
    sphere.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    sphere.fillMode = kFillModeSolid;

    //==================================================
    // 正四面体
    //==================================================

    std::vector<std::unique_ptr<AnimationTetrahedron>> tetrahedrons;
    // 正四面体のデフォルトの設定
    Tetrahedron defaultTetrahedron;
    defaultTetrahedron.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 2.0f, 0.0f }
    };
    defaultTetrahedron.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    defaultTetrahedron.material.enableLighting = true;
    // カメラを設定
    defaultTetrahedron.camera = camera.get();
    // テクスチャを設定
    defaultTetrahedron.useTextureIndex = -1;
    // 法線の種類
    defaultTetrahedron.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    defaultTetrahedron.fillMode = kFillModeSolid;

    //==================================================
    // 床用の板
    //==================================================

    Plane floor(camera.get());
    // 左前
    floor.mesh->vertexBufferMap[0].position = { -5.0f, 0.0f, -5.0f, 1.0f };
    floor.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 左奥
    floor.mesh->vertexBufferMap[1].position = { -5.0f, 0.0f, 5.0f, 1.0f };
    floor.mesh->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    // 右前
    floor.mesh->vertexBufferMap[2].position = { 5.0f, 0.0f, -5.0f, 1.0f };
    floor.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // 右奥
    floor.mesh->vertexBufferMap[3].position = { 5.0f, 0.0f, 5.0f, 1.0f };
    floor.mesh->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
    floor.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    floor.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    floor.material.enableLighting = true;
    // テクスチャを設定
    floor.useTextureIndex = textures[0];
    // 法線の種類
    floor.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    floor.fillMode = kFillModeSolid;

    // ウィンドウのxボタンが押されるまでループ
    while (myGameEngine->ProccessMessage() != -1) {
        myGameEngine->BeginFrame();
        Input::Update();

        //==================================================
        // 更新処理
        //==================================================

        // F3キーでデバッグカメラの有効化
        if (Input::IsKeyTrigger(DIK_F3)) {
            isUseDebugCamera = !isUseDebugCamera;
            drawer->ToggleDebugCamera();
        }

        // 1 ～ 6 でブレンドモードを変更
        if (Input::IsKeyTrigger(DIK_1)) {
            blendMode = kBlendModeNone;
        } else if (Input::IsKeyTrigger(DIK_2)) {
            blendMode = kBlendModeNormal;
        } else if (Input::IsKeyTrigger(DIK_3)) {
            blendMode = kBlendModeAdd;
        } else if (Input::IsKeyTrigger(DIK_4)) {
            blendMode = kBlendModeSubtract;
        } else if (Input::IsKeyTrigger(DIK_5)) {
            blendMode = kBlendModeMultiply;
        } else if (Input::IsKeyTrigger(DIK_6)) {
            blendMode = kBlendModeScreen;
        }

        //ImGuiManager::Begin("オブジェクト");

        //// カメラ位置の表示
        //ImGui::Text("カメラ位置: (%.2f, %.2f, %.2f)", camera->GetTranslate().x, camera->GetTranslate().y, camera->GetTranslate().z);
        //// カメラの回転の表示
        //ImGui::Text("カメラの回転: (%.2f, %.2f, %.2f)", camera->GetRotate().x, camera->GetRotate().y, camera->GetRotate().z);
        //// ブレンドモードの表示
        //ImGui::Text("ブレンドモード: %d", static_cast<int>(blendMode));

        //// デバッグカメラの有効化
        //if (ImGui::Checkbox("デバッグカメラ有効化", &isUseDebugCamera)) {
        //    drawer->ToggleDebugCamera();
        //}

        //// 背景色
        //ImGui::DragFloat4("背景色", &clearColor.x, 1.0f, 0.0f, 255.0f);

        //// 平行光源
        //if (ImGui::TreeNode("平行光源")) {
        //    ImGui::DragFloat3("DirectionalLight Direction", &directionalLight.direction.x, 0.01f);
        //    ImGui::DragFloat4("DirectionalLight Color", &directionalLight.color.x, 1.0f, 0.0f, 255.0f);
        //    ImGui::DragFloat("DirectionalLight Intensity", &directionalLight.intensity, 0.01f);
        //    ImGui::TreePop();
        //}

        //// 球体
        //if (ImGui::TreeNode("球体")) {
        //    ImGui::DragFloat3("Sphere Translate", &sphere.transform.translate.x, 0.01f);
        //    ImGui::DragFloat3("Sphere Rotate", &sphere.transform.rotate.x, 0.01f);
        //    ImGui::DragFloat3("Sphere Scale", &sphere.transform.scale.x, 0.01f);
        //    ImGui::DragFloat4("Sphere MaterialColor", &sphere.material.color.x, 1.0f, 0.0f, 255.0f);
        //    ImGui::InputInt("Sphere TextureIndex", &sphere.useTextureIndex);
        //    if (ImGui::TreeNode("Sphere uvTransform")) {
        //        ImGui::DragFloat2("Sphere uvTransform Translate", &sphere.uvTransform.translate.x, 0.01f);
        //        ImGui::DragFloat3("Sphere uvTransform Rotate", &sphere.uvTransform.rotate.x, 0.01f);
        //        ImGui::DragFloat2("Sphere uvTransform Scale", &sphere.uvTransform.scale.x, 0.01f);
        //        ImGui::TreePop();
        //    }
        //    ImGui::TreePop();
        //}

        //// 正四面体
        //if (ImGui::TreeNode("正四面体")) {
        //    ImGui::DragFloat3("Tetrahedron Translate", &defaultTetrahedron.transform.translate.x, 0.01f);
        //    ImGui::DragFloat3("Tetrahedron Rotate", &defaultTetrahedron.transform.rotate.x, 0.01f);
        //    ImGui::DragFloat3("Tetrahedron Scale", &defaultTetrahedron.transform.scale.x, 0.01f);
        //    ImGui::DragFloat4("Tetrahedron MaterialColor", &defaultTetrahedron.material.color.x, 1.0f, 0.0f, 255.0f);
        //    ImGui::InputInt("Tetrahedron TextureIndex", &defaultTetrahedron.useTextureIndex);
        //    if (ImGui::TreeNode("Tetrahedron uvTransform")) {
        //        ImGui::DragFloat2("Tetrahedron uvTransform Translate", &defaultTetrahedron.uvTransform.translate.x, 0.01f);
        //        ImGui::DragFloat3("Tetrahedron uvTransform Rotate", &defaultTetrahedron.uvTransform.rotate.x, 0.01f);
        //        ImGui::DragFloat2("Tetrahedron uvTransform Scale", &defaultTetrahedron.uvTransform.scale.x, 0.01f);
        //        ImGui::TreePop();
        //    }
        //    ImGui::TreePop();
        //}

        //// 板
        //if (ImGui::TreeNode("板")) {
        //    ImGui::DragFloat3("Plane Translate", &floor.transform.translate.x, 0.01f);
        //    ImGui::DragFloat3("Plane Rotate", &floor.transform.rotate.x, 0.01f);
        //    ImGui::DragFloat3("Plane Scale", &floor.transform.scale.x, 0.01f);
        //    ImGui::DragFloat4("Plane MaterialColor", &floor.material.color.x, 1.0f, 0.0f, 255.0f);
        //    ImGui::InputInt("Plane TextureIndex", &floor.useTextureIndex);
        //    if (ImGui::TreeNode("Plane uvTransform")) {
        //        ImGui::DragFloat2("Plane uvTransform Translate", &floor.uvTransform.translate.x, 0.01f);
        //        ImGui::DragFloat3("Plane uvTransform Rotate", &floor.uvTransform.rotate.x, 0.01f);
        //        ImGui::DragFloat2("Plane uvTransform Scale", &floor.uvTransform.scale.x, 0.01f);
        //        ImGui::TreePop();
        //    }
        //    ImGui::TreePop();
        //}

        //ImGui::End();

        // カメラを常に回転
        if (!drawer->IsUseDebugCamera()) {
            camera->GetRotatePtr()->y -= 0.01f;
        }

        //--------- 正四面体の更新 ---------//

        for (auto tetrahedron = tetrahedrons.begin(); tetrahedron != tetrahedrons.end(); ) {
            // 移動
            (*tetrahedron)->tetrahedron.transform.translate +=
                (*tetrahedron)->animationTransform.translate * 0.01f;
            // 回転
            (*tetrahedron)->tetrahedron.transform.rotate +=
                (*tetrahedron)->animationTransform.rotate * 0.01f;
            // 生存時間によってスケールを小さくする
            (*tetrahedron)->tetrahedron.transform.scale.x =
                1.0f - (static_cast<float>((*tetrahedron)->aliveTimer) / static_cast<float>(kAliveTime));
            (*tetrahedron)->tetrahedron.transform.scale.y =
                1.0f - (static_cast<float>((*tetrahedron)->aliveTimer) / static_cast<float>(kAliveTime));
            (*tetrahedron)->tetrahedron.transform.scale.z =
                1.0f - (static_cast<float>((*tetrahedron)->aliveTimer) / static_cast<float>(kAliveTime));

            // 生存時間をカウント
            (*tetrahedron)->aliveTimer++;
            // 生存時間が経過したら削除
            if ((*tetrahedron)->aliveTimer >= kAliveTime) {
                tetrahedron = tetrahedrons.erase(tetrahedron);
            } else {
                ++tetrahedron;
            }
        }

        //--------- 正四面体の生成 ---------//

        // 正四面体の出現タイマーをカウント
        spawnTimer++;
        // 正四面体の出現タイマーが経過したら生成
        if (spawnTimer >= kWaitTime) {
            // 正四面体を生成
            AnimationTetrahedron animationTetrahedron;
            animationTetrahedron.tetrahedron.transform = defaultTetrahedron.transform;
            animationTetrahedron.tetrahedron.material = defaultTetrahedron.material;
            animationTetrahedron.tetrahedron.useTextureIndex = defaultTetrahedron.useTextureIndex;
            animationTetrahedron.tetrahedron.fillMode = defaultTetrahedron.fillMode;
            animationTetrahedron.tetrahedron.normalType = defaultTetrahedron.normalType;
            animationTetrahedron.tetrahedron.camera = camera.get();
            animationTetrahedron.aliveTimer = 0;

            // 正四面体の進行方向と回転をランダムに設定
            animationTetrahedron.animationTransform.translate = {
                rand(engine),
                rand(engine),
                rand(engine)
            };
            animationTetrahedron.animationTransform.translate =
                animationTetrahedron.animationTransform.translate.Normalize();
            animationTetrahedron.animationTransform.rotate = {
                rand(engine),
                rand(engine),
                rand(engine)
            };
            tetrahedrons.push_back(std::make_unique<AnimationTetrahedron>(animationTetrahedron));

            // 正四面体の出現タイマーをリセット
            spawnTimer = 0;
        }

        //==================================================
        // 描画処理
        //==================================================

        // 背景色を設定
        dxCommon->SetClearColor(ConvertColor(clearColor));
        // ブレンドモードを設定
        drawer->SetBlendMode(blendMode);
        // 平行光源を設定
        drawer->SetLight(&directionalLight);

        // 球体の描画
        drawer->DrawSet(&sphere);
        // 板の描画
        drawer->DrawSet(&floor);
        // 正四面体の描画
        for (auto &tetrahedron : tetrahedrons) {
            drawer->DrawSet(&tetrahedron->tetrahedron);
        }
        drawer->DrawExecute();
        
        myGameEngine->EndFrame();

        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }

    return 0;
}