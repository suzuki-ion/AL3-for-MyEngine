#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <filesystem>

#include "Common/VertexData.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"

namespace MyEngine {

// 前方宣言
class WinApp;
class DirectXCommon;
class Drawer;
class TextureManager;

} // namespace MyEngine

/// @brief 自作エンジンクラス
class Engine final {
public:
    Engine(const char *title, int width = 1280, int height = 720, bool enableDebugLayer = true,
        const std::filesystem::path &projectDir = std::filesystem::current_path());
    ~Engine();

    /// @brief フレーム開始処理
    void BeginFrame();

    /// @brief ゲームループ開始処理
    /// @return ゲームループを開始するかどうか
    bool BeginGameLoop();

    /// @brief フレーム終了処理
    void EndFrame();

    /// @brief デルタタイム取得
    /// @return デルタタイム
    float GetDeltaTime() const;

    /// @brief フレームレート取得
    /// @return フレームレート
    unsigned int GetFPS() const;

    /// @brief WinAppクラスのポインタ取得
    /// @return WinAppクラスのポインタ
    MyEngine::WinApp *GetWinApp() const;

    /// @brief DirectXCommonクラスのポインタ取得
    /// @return DirectXCommonクラスのポインタ
    MyEngine::DirectXCommon *GetDxCommon() const;

    /// @brief 描画用クラスのポインタ取得
    /// @return 描画用クラスのポインタ
    MyEngine::Drawer *GetDrawer() const;

    /// @brief テクスチャ管理クラスのポインタ取得
    /// @return テクスチャ管理クラスのポインタ
    MyEngine::TextureManager *GetTextureManager() const;
    
    /// @brief メッセージ処理
    /// @return メッセージ処理結果。-1の場合は終了
    int ProccessMessage();

private:
};