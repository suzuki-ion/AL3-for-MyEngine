#include <Engine.h>
#include <memory>
#include <imgui.h>

#include "Math/Camera.h"
#include "Objects/Triangle.h"
#include "Objects/Sprite.h"
#include "Objects/Sphere.h"
#include "Objects/Billboard.h"

using namespace MyEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> engine = std::make_unique<Engine>("MyEngine", 1280, 720, true);

    // 描画用クラスへのポインタ
    Drawer *drawer = engine->GetDrawer();

    //==================================================
    // カメラ
    //==================================================

    std::unique_ptr<Camera> camera = std::make_unique<Camera>(
        static_cast<float>(engine->GetClientWidth()),
        static_cast<float>(engine->GetClientHeight()),
        Vector3( 0.0f, 0.0f, -5.0f ),
        Vector3( 0.0f, 0.0f, 0.0f ),
        Vector3( 1.0f, 1.0f, 1.0f )
    );

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
    triangle1.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    // 左下
    triangle1.vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
    triangle1.vertexData[0].texCoord = { 0.0f, 1.0f };
    // 上
    triangle1.vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
    triangle1.vertexData[1].texCoord = { 0.5f, 0.0f };
    // 右下
    triangle1.vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    triangle1.vertexData[2].texCoord = { 1.0f, 1.0f };
    // カメラを設定
    triangle1.camera = camera.get();

    //--------- 三角形2 ---------//

    Triangle triangle2;
    triangle2.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    triangle2.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    // 左下
    triangle2.vertexData[0].position = { -0.5f, -0.5f, 0.5f, 1.0f };
    triangle2.vertexData[0].texCoord = { 0.0f, 1.0f };
    // 上
    triangle2.vertexData[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    triangle2.vertexData[1].texCoord = { 0.5f, 0.0f };
    // 右下
    triangle2.vertexData[2].position = { 0.5f, -0.5f, -0.5f, 1.0f };
    triangle2.vertexData[2].texCoord = { 1.0f, 1.0f };
    // カメラを設定
    triangle2.camera = camera.get();

    //==================================================
    // スプライト
    //==================================================

    Sprite sprite;
    sprite.transform = {
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
    sprite.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    // 左上
    sprite.vertexData[0].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    sprite.vertexData[0].texCoord = { 0.0f, 0.0f };
    // 右上
    sprite.vertexData[1].position = { 640.0f, 0.0f, 0.0f, 1.0f };
    sprite.vertexData[1].texCoord = { 1.0f, 0.0f };
    // 左下
    sprite.vertexData[2].position = { 0.0f, 320.0f, 0.0f, 1.0f };
    sprite.vertexData[2].texCoord = { 0.0f, 1.0f };
    // 右下
    sprite.vertexData[3].position = { 640.0f, 320.0f, 0.0f, 1.0f };
    sprite.vertexData[3].texCoord = { 1.0f, 1.0f };

    //==================================================
    // 球体
    //==================================================

    Sphere sphere(16);
    sphere.transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };
    sphere.color = { 255.0f, 255.0f, 255.0f, 255.0f };
    sphere.radius = 1.0f;
    // カメラを設定
    sphere.camera = camera.get();
    // テクスチャを設定
    sphere.useTextureIndex = 0;

    // ウィンドウのxボタンが押されるまでループ
    while (engine->ProccessMessage() != -1) {
        engine->BeginFrame();

        // ImGuiで三角形をいじれるようにする
        ImGui::Begin("Triangle1");
        ImGui::DragFloat3("Triangle1 Translate", &triangle1.transform.translate.x, 0.01f);
        ImGui::DragFloat3("Triangle1 Rotate", &triangle1.transform.rotate.x, 0.01f);
        ImGui::DragFloat3("Triangle1 Scale", &triangle1.transform.scale.x, 0.01f);
        ImGui::DragFloat4("Triangle1 MaterialColor", &triangle1.color.x, 1.0f, 0.0f, 255.0f);
        ImGui::InputInt("Triangle1 TextureIndex", &triangle1.useTextureIndex);
        ImGui::End();

        ImGui::Begin("Triangle2");
        ImGui::DragFloat3("Triangle2 Translate", &triangle2.transform.translate.x, 0.01f);
        ImGui::DragFloat3("Triangle2 Rotate", &triangle2.transform.rotate.x, 0.01f);
        ImGui::DragFloat3("Triangle2 Scale", &triangle2.transform.scale.x, 0.01f);
        ImGui::DragFloat4("Triangle2 MaterialColor", &triangle2.color.x, 1.0f, 0.0f, 255.0f);
        ImGui::InputInt("Triangle2 TextureIndex", &triangle2.useTextureIndex);
        ImGui::End();

        // ImGuiでスプライトをいじれるようにする
        ImGui::Begin("Sprite");
        ImGui::DragFloat3("Sprite Translate", &sprite.transform.translate.x, 0.01f);
        ImGui::DragFloat3("Sprite Rotate", &sprite.transform.rotate.x, 0.01f);
        ImGui::DragFloat3("Sprite Scale", &sprite.transform.scale.x, 0.01f);
        ImGui::DragFloat4("Sprite MaterialColor", &sprite.color.x, 1.0f, 0.0f, 255.0f);
        ImGui::InputInt("Sprite TextureIndex", &sprite.useTextureIndex);
        ImGui::End();

        // ImGuiで球体をいじれるようにする
        ImGui::Begin("Sphere");
        ImGui::DragFloat3("Sphere Translate", &sphere.transform.translate.x, 0.01f);
        ImGui::DragFloat3("Sphere Rotate", &sphere.transform.rotate.x, 0.01f);
        ImGui::DragFloat3("Sphere Scale", &sphere.transform.scale.x, 0.01f);
        ImGui::DragFloat("Sphere Radius", &sphere.radius, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat4("Sphere MaterialColor", &sphere.color.x, 1.0f, 0.0f, 255.0f);
        ImGui::InputInt("Sphere TextureIndex", &sphere.useTextureIndex);
        ImGui::End();

        // カメラをいじれるようにする
        camera->MoveToMouse(0.01f, 0.01f, 0.1f);

        drawer->Draw(&triangle1);
        drawer->Draw(&triangle2);
        drawer->Draw(&sphere);
        drawer->Draw(&sprite);

        engine->EndFrame();
    }

    return 0;
}