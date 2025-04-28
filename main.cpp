#include <Engine.h>
#include <memory>
#include <imgui.h>

#include "Common/VertexData.h"
#include "Math/Transform.h"

using namespace MyEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> engine = std::make_unique<Engine>("MyEngine", 1280, 720, true);

    //==================================================
    // 三角形
    //==================================================

    // 変形用のtransform
    Transform transformTriangle{
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

    Vector4 colorTriangle{ 255.0f, 255.0f, 255.0f, 255.0f };

    //--------- 三角形1 ---------//

    // 頂点リソース
    VertexData vertexDataTriangle1[3];

    // 左下
    vertexDataTriangle1[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
    vertexDataTriangle1[0].texCoord = { 0.0f, 1.0f };
    // 上
    vertexDataTriangle1[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
    vertexDataTriangle1[1].texCoord = { 0.5f, 0.0f };
    // 右下
    vertexDataTriangle1[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    vertexDataTriangle1[2].texCoord = { 1.0f, 1.0f };

    //--------- 三角形2 ---------//

    // 頂点リソース
    VertexData vertexDataTriangle2[3];

    // 左上
    vertexDataTriangle2[0].position = { -0.5f, -0.5f, 0.5f, 1.0f };
    vertexDataTriangle2[0].texCoord = { 0.0f, 1.0f };
    // 右上
    vertexDataTriangle2[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexDataTriangle2[1].texCoord = { 0.5f, 0.0f };
    // 右下
    vertexDataTriangle2[2].position = { 0.5f, -0.5f, -0.5f, 1.0f };
    vertexDataTriangle2[2].texCoord = { 1.0f, 1.0f };

    //==================================================
    // スプライト
    //==================================================

    // 変形用のtransform
    Transform transformSprite{
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

    Vector4 colorSprite{ 255.0f, 255.0f, 255.0f, 255.0f };

    // 頂点リソース
    VertexData vertexDataSprite[4];

    // 左上
    vertexDataSprite[0].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[0].texCoord = { 0.0f, 0.0f };
    // 右上
    vertexDataSprite[1].position = { 640.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[1].texCoord = { 1.0f, 0.0f };
    // 左下
    vertexDataSprite[2].position = { 0.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[2].texCoord = { 0.0f, 1.0f };
    // 右下
    vertexDataSprite[3].position = { 640.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[3].texCoord = { 1.0f, 1.0f };

    // ウィンドウのxボタンが押されるまでループ
    while (engine->ProccessMessage() != -1) {
        engine->BeginFrame();

        // ImGuiで三角形をいじれるようにする
        ImGui::Begin("Triangle");
        ImGui::DragFloat3("Triangle Translate", &transformTriangle.translate.x, 0.1f);
        ImGui::DragFloat3("Triangle Rotate", &transformTriangle.rotate.x, 0.01f);
        ImGui::DragFloat3("Triangle Scale", &transformTriangle.scale.x, 0.01f);
        ImGui::DragFloat4("Triangle MaterialColor", &colorTriangle.x, 1.0f, 0.0f, 255.0f);
        ImGui::End();

        // ImGuiでスプライトをいじれるようにする
        ImGui::Begin("Sprite");
        ImGui::DragFloat3("Sprite Translate", &transformSprite.translate.x, 0.1f);
        ImGui::DragFloat3("Sprite Rotate", &transformSprite.rotate.x, 0.01f);
        ImGui::DragFloat3("Sprite Scale", &transformSprite.scale.x, 0.01f);
        ImGui::DragFloat4("Sprite MaterialColor", &colorSprite.x, 1.0f, 0.0f, 255.0f);
        ImGui::End();

        engine->DrawTest(
            vertexDataTriangle1,
            vertexDataTriangle2,
            transformTriangle,
            colorTriangle
        );

        engine->DrawSpriteTest(
            vertexDataSprite,
            transformSprite,
            colorSprite
        );

        engine->EndFrame();
    }

    return 0;
}