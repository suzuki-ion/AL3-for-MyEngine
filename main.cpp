#include <Engine.h>
#include <memory>
#include <imgui.h>

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/Drawer.h"
#include "Base/TextureManager.h"
#include "Base/Input.h"

#include "Math/Camera.h"
#include "Common/ConvertColor.h"

#include "3d/DirectionalLight.h"
#include "Objects/Triangle.h"
#include "Objects/Sprite.h"
#include "Objects/Sphere.h"
#include "Objects/Billboard.h"
#include "Objects/ModelData.h"

using namespace MyEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> engine = std::make_unique<Engine>("MyEngine", 1280, 720, true);

    // WinAppクラスへのポインタ
    WinApp *winApp = engine->GetWinApp();
    winApp->SetSizeChangeMode(SizeChangeMode::kFixedAspect);
    // DirectXCommonクラスへのポインタ
    DirectXCommon *dxCommon = engine->GetDxCommon();
    // 描画用クラスへのポインタ
    Drawer *drawer = engine->GetDrawer();
    // テクスチャ管理クラスへのポインタ
    TextureManager *textureManager = engine->GetTextureManager();

    // テクスチャを読み込む
    uint32_t textures[3];
    textures[1] = textureManager->Load("Resources/uvChecker.png");
    textures[2] = textureManager->Load("Resources/monsterBall.png");

    // 塗りつぶしフラグ
    bool isFillMode = true;
    // ブレンドモード
    BlendMode blendMode = kBlendModeNormal;

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera = std::make_unique<Camera>(
        winApp,
        Vector3( 0.0f, 0.0f, -5.0f ),
        Vector3( 0.0f, 0.0f, 0.0f ),
        Vector3( 1.0f, 1.0f, 1.0f )
    );
    // デバッグカメラの有効化フラグ
    bool isUseDebugCamera = true;

    //==================================================
    // 背景の色
    //==================================================

    Vector4 clearColor = { 128.0f, 192.0f, 256.0f, 255.0f };

    //==================================================
    // 平行光源
    //==================================================

    DirectionalLight directionalLight = {
        { 255.0f, 192.0f, 224.0f, 255.0f },
        { 0.5f, -0.75f, 0.5f },
        1.0f
    };

    //==================================================
    // 三角形
    //==================================================

    //--------- 三角形1 ---------//

    Triangle triangle1;
    triangle1.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    triangle1.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    triangle1.material.enableLighting = true;
    // 左下
    triangle1.mesh->vertexBufferMap[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
    triangle1.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 上
    triangle1.mesh->vertexBufferMap[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
    triangle1.mesh->vertexBufferMap[1].texCoord = { 0.5f, 0.0f };
    // 右下
    triangle1.mesh->vertexBufferMap[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    triangle1.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // カメラを設定
    triangle1.camera = camera.get();
    // テクスチャを設定
    triangle1.useTextureIndex = 0;
    // 法線の種類
    triangle1.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    triangle1.fillMode = kFillModeSolid;

    //--------- 三角形2 ---------//

    Triangle triangle2;
    triangle2.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    triangle2.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    triangle2.material.enableLighting = true;
    // 左下
    triangle2.mesh->vertexBufferMap[0].position = { -0.5f, -0.5f, 0.5f, 1.0f };
    triangle2.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 上
    triangle2.mesh->vertexBufferMap[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    triangle2.mesh->vertexBufferMap[1].texCoord = { 0.5f, 0.0f };
    // 右下
    triangle2.mesh->vertexBufferMap[2].position = { 0.5f, -0.5f, -0.5f, 1.0f };
    triangle2.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // カメラを設定
    triangle2.camera = camera.get();
    // テクスチャを設定
    triangle2.useTextureIndex = 0;
    // 法線の種類
    triangle2.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    triangle2.fillMode = kFillModeWireframe;

    //==================================================
    // スプライト
    //==================================================

    Sprite sprite;
    sprite.transform = {
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
    sprite.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    sprite.material.enableLighting = false;
    // 左下
    sprite.mesh->vertexBufferMap[0].position = { 0.0f, 320.0f, 0.0f, 1.0f };
    sprite.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 左上
    sprite.mesh->vertexBufferMap[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    sprite.mesh->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    // 右下
    sprite.mesh->vertexBufferMap[2].position = { 640.0f, 320.0f, 0.0f, 1.0f };
    sprite.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // 右上
    sprite.mesh->vertexBufferMap[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
    sprite.mesh->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };

    //==================================================
    // 球体
    //==================================================

    Sphere sphere(16);
    sphere.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    sphere.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    sphere.material.enableLighting = true;
    sphere.radius = 1.0f;
    // カメラを設定
    sphere.camera = camera.get();
    // テクスチャを設定
    sphere.useTextureIndex = 0;
    // 法線の種類
    sphere.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    sphere.fillMode = kFillModeSolid;

    //==================================================
    // ビルボード
    //==================================================

    BillBoard billboard(camera.get());
    billboard.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    billboard.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    billboard.material.enableLighting = true;
    // 左下
    billboard.mesh->vertexBufferMap[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
    billboard.mesh->vertexBufferMap[0].texCoord = { 0.0f, 1.0f };
    // 左上
    billboard.mesh->vertexBufferMap[1].position = { -0.5f, 0.5f, 0.0f, 1.0f };
    billboard.mesh->vertexBufferMap[1].texCoord = { 0.0f, 0.0f };
    // 右下
    billboard.mesh->vertexBufferMap[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    billboard.mesh->vertexBufferMap[2].texCoord = { 1.0f, 1.0f };
    // 右上
    billboard.mesh->vertexBufferMap[3].position = { 0.5f, 0.5f, 0.0f, 1.0f };
    billboard.mesh->vertexBufferMap[3].texCoord = { 1.0f, 0.0f };
    // テクスチャを設定
    billboard.useTextureIndex = 0;
    // 法線の種類
    billboard.normalType = kNormalTypeFace;
    // 塗りつぶしモードを設定
    billboard.fillMode = kFillModeSolid;

    //==================================================
    // モデル
    //==================================================

    //ModelData modelData(
    //    "Resources",
    //    "plane.obj",
    //    textureManager
    //);
    //modelData.transform = {
    //    { 1.0f, 1.0f, 1.0f },
    //    { 0.0f, 0.0f, 0.0f },
    //    { 0.0f, 0.0f, 0.0f }
    //};
    //modelData.material.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    //modelData.material.enableLighting = true;
    //// カメラを設定
    //modelData.camera = camera.get();
    //// 塗りつぶしモードを設定
    //modelData.fillMode = kFillModeSolid;

    // ウィンドウのxボタンが押されるまでループ
    while (engine->ProccessMessage() != -1) {
        engine->BeginFrame();
        Input::Update();

        // F3キーでデバッグカメラの有効化
        if (Input::IsKeyTrigger(DIK_F3)) {
            isUseDebugCamera = !isUseDebugCamera;
            drawer->ToggleDebugCamera();
        }

        // 1 ～ 7 でブレンドモードを変更
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
        } else if (Input::IsKeyTrigger(DIK_7)) {
            blendMode = kBlendModeExclusion;
        }

        ImGui::Begin("Objects");

        // デバッグカメラの有効化
        if (ImGui::Checkbox("DebugCamera", &isUseDebugCamera)) {
            drawer->ToggleDebugCamera();
        }

        // 背景色
        ImGui::DragFloat4("ClearColor", &clearColor.x, 1.0f, 0.0f, 255.0f);

        // 平行光源
        if (ImGui::TreeNode("Directional Light")) {
            ImGui::DragFloat3("DirectionalLight Direction", &directionalLight.direction.x, 0.01f);
            ImGui::DragFloat4("DirectionalLight Color", &directionalLight.color.x, ImGuiColorEditFlags_Uint8);
            ImGui::DragFloat("DirectionalLight Intensity", &directionalLight.intensity, 0.01f, 0.0f, 1.0f);
            ImGui::TreePop();
        }

        // 三角形1
        if (ImGui::TreeNode("Triangle1")) {
            ImGui::DragFloat3("Triangle1 Translate", &triangle1.transform.translate.x, 0.01f);
            ImGui::DragFloat3("Triangle1 Rotate", &triangle1.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("Triangle1 Scale", &triangle1.transform.scale.x, 0.01f);
            ImGui::DragFloat4("Triangle1 MaterialColor", &triangle1.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Triangle1 TextureIndex", &triangle1.useTextureIndex);
            if (ImGui::TreeNode("Triangle1 uvTransform")) {
                ImGui::DragFloat2("Triangle1 uvTransform Translate", &triangle1.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("Triangle1 uvTransform Rotate", &triangle1.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("Triangle1 uvTransform Scale", &triangle1.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        // 三角形2
        if (ImGui::TreeNode("Triangle2")) {
            ImGui::DragFloat3("Triangle2 Translate", &triangle2.transform.translate.x, 0.01f);
            ImGui::DragFloat3("Triangle2 Rotate", &triangle2.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("Triangle2 Scale", &triangle2.transform.scale.x, 0.01f);
            ImGui::DragFloat4("Triangle2 MaterialColor", &triangle2.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Triangle2 TextureIndex", &triangle2.useTextureIndex);
            if (ImGui::TreeNode("Triangle2 uvTransform")) {
                ImGui::DragFloat2("Triangle2 uvTransform Translate", &triangle2.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("Triangle2 uvTransform Rotate", &triangle2.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("Triangle2 uvTransform Scale", &triangle2.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        // スプライト
        if (ImGui::TreeNode("Sprite")) {
            ImGui::DragFloat3("Sprite Translate", &sprite.transform.translate.x, 0.01f);
            ImGui::DragFloat3("Sprite Rotate", &sprite.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("Sprite Scale", &sprite.transform.scale.x, 0.01f);
            ImGui::DragFloat4("Sprite MaterialColor", &sprite.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Sprite TextureIndex", &sprite.useTextureIndex);
            if (ImGui::TreeNode("Sprite uvTransform")) {
                ImGui::DragFloat2("Sprite uvTransform Translate", &sprite.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("Sprite uvTransform Rotate", &sprite.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("Sprite uvTransform Scale", &sprite.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        // 球体
        if (ImGui::TreeNode("Sphere")) {
            ImGui::DragFloat3("Sphere Translate", &sphere.transform.translate.x, 0.01f);
            ImGui::DragFloat3("Sphere Rotate", &sphere.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("Sphere Scale", &sphere.transform.scale.x, 0.01f);
            ImGui::DragFloat("Sphere Radius", &sphere.radius, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat4("Sphere MaterialColor", &sphere.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Sphere TextureIndex", &sphere.useTextureIndex);
            if (ImGui::TreeNode("Sphere uvTransform")) {
                ImGui::DragFloat2("Sphere uvTransform Translate", &sphere.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("Sphere uvTransform Rotate", &sphere.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("Sphere uvTransform Scale", &sphere.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        // ビルボード
        if (ImGui::TreeNode("Billboard")) {
            ImGui::DragFloat3("Billboard Translate", &billboard.transform.translate.x, 0.01f);
            ImGui::DragFloat3("Billboard Rotate", &billboard.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("Billboard Scale", &billboard.transform.scale.x, 0.01f);
            ImGui::DragFloat4("Billboard MaterialColor", &billboard.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("Billboard TextureIndex", &billboard.useTextureIndex);
            if (ImGui::TreeNode("Billboard uvTransform")) {
                ImGui::DragFloat2("Billboard uvTransform Translate", &billboard.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("Billboard uvTransform Rotate", &billboard.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("Billboard uvTransform Scale", &billboard.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        // モデル
        /*if (ImGui::TreeNode("ModelData")) {
            ImGui::DragFloat3("ModelData Translate", &modelData.transform.translate.x, 0.01f);
            ImGui::DragFloat3("ModelData Rotate", &modelData.transform.rotate.x, 0.01f);
            ImGui::DragFloat3("ModelData Scale", &modelData.transform.scale.x, 0.01f);
            ImGui::DragFloat4("ModelData MaterialColor", &modelData.material.color.x, 1.0f, 0.0f, 255.0f);
            ImGui::InputInt("ModelData TextureIndex", &modelData.useTextureIndex);
            if (ImGui::TreeNode("ModelData uvTransform")) {
                ImGui::DragFloat2("ModelData uvTransform Translate", &modelData.uvTransform.translate.x, 0.01f);
                ImGui::DragFloat3("ModelData uvTransform Rotate", &modelData.uvTransform.rotate.x, 0.01f);
                ImGui::DragFloat2("ModelData uvTransform Scale", &modelData.uvTransform.scale.x, 0.01f);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }*/

        ImGui::End();

        // カメラをいじれるようにする
        if (!drawer->IsUseDebugCamera()) {
            camera->MoveToMouse(0.01f, 0.01f, 0.1f);
        }

        // 背景色を設定
        dxCommon->SetClearColor(ConvertColor(clearColor));

        // ブレンドモードを設定
        drawer->SetBlendMode(blendMode);

        // 平行光源を設定
        //drawer->SetLight(&directionalLight);

        drawer->DrawSet(&triangle1);
        drawer->DrawSet(&triangle2);
        drawer->DrawSet(&sphere);
        drawer->DrawSet(&billboard);
        //drawer->DrawSet(&sprite);
        //drawer->DrawSet(&modelData);

        engine->EndFrame();
    }

    return 0;
}