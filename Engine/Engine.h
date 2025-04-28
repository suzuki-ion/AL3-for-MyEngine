#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <filesystem>

#include "Common/VertexData.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"

/// @brief 自作エンジンクラス
class Engine final {
public:
    Engine(const char *title, int width = 1280, int height = 720, bool enableDebugLayer = true,
        const std::filesystem::path &projectDir = std::filesystem::current_path());
    ~Engine();

    /// @brief フレーム開始処理
    void BeginFrame();

    /// @brief フレーム終了処理
    void EndFrame();

    /// @brief テスト描画
    /// @param vertexData 三角形1に使う描画に使う頂点データ
    /// @param vertexData2 三角形2に使う描画に使う頂点データ
    /// @param color 色
    void DrawTest(
        const MyEngine::VertexData (&vertexData1)[3],
        const MyEngine::VertexData (&vertexData2)[3],
        const MyEngine::Transform &transform,
        const MyEngine::Vector4 &color
    );

    /// @brief スプライトのテスト描画
    void DrawSpriteTest(
        const MyEngine::VertexData(&vertexData)[4],
        const MyEngine::Transform &transform,
        const MyEngine::Vector4 &color
    );

    /// @brief ウィンドウハンドル取得
    HWND GetWindowHandle() const;

    /// @brief クライアントサイズの横幅取得
    /// @return クライアントサイズの横幅
    int32_t GetClientWidth() const;
    
    /// @brief クライアントサイズの縦幅取得
    /// @return クライアントサイズの縦幅
    int32_t GetClientHeight() const;
    
    /// @brief メッセージ処理
    /// @return メッセージ処理結果。-1の場合は終了
    int ProccessMessage();

private:
};