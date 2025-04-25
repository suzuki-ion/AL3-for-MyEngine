#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <filesystem>

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
    void DrawTest();

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