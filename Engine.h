#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

/// @brief Windowsアプリクラスの前方宣言
class WinApp;

/// @brief 自作エンジンクラス
class Engine final {
public:
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

    /// @brief インスタンス取得
    /// @return エンジンクラスのインスタンス
    static Engine *GetInstance() {
        static Engine instance;
        return &instance;
    }

    /// @brief エンジン初期化
    void Initialize();

    /// @brief エンジン終了処理
    void Finalize();
    
    /// @brief ウィンドウハンドル取得
    HWND GetWindowHandle() const;

    /// @brief クライアントサイズの横幅取得
    /// @return クライアントサイズの横幅
    int32_t GetClientWidth() const;
    
    /// @brief クライアントサイズの縦幅取得
    /// @return クライアントサイズの縦幅
    int32_t GetClientHeight() const;
    
    /// @brief メッセージ処理
    /// @return メッセージ処理結果
    int ProccessMessage();

private:
    Engine() = default;
    ~Engine() = default;

    /// @brief  Windowsアプリクラス
    WinApp *winApp_ = nullptr;
};