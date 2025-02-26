#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

/// @brief Windowsアプリクラス
class WinApp final {
public:
    WinApp(const WinApp &) = delete;
    WinApp &operator=(const WinApp &) = delete;
    WinApp(WinApp &&) = delete;
    WinApp &operator=(WinApp &&) = delete;

    /// @brief インスタンス取得
    /// @return Windowsアプリクラスのインスタンス
    static WinApp *GetInstance() {
        static WinApp instance;
        return &instance;
    }

    /// @brief Windowsアプリ初期化
    void Initialize();

    /// @brief ウィンドウハンドル取得
    /// @return ウィンドウハンドル
    HWND GetWindowHandle() const { return hwnd_; }

    /// @brief クライアントサイズの横幅取得
    /// @return クライアントサイズの横幅
    int32_t GetClientWidth() const { return kClientWidth_; }

    /// @brief クライアントサイズの縦幅取得
    /// @return クライアントサイズの縦幅
    int32_t GetClientHeight() const { return kClientHeight_; }

    /// @brief メッセージ処理
    /// @return メッセージ処理結果
    int ProccessMessage();

    /// @brief ログ出力
    void Log(const std::string &message);
    /// @brief ログ出力
    void Log(const std::wstring &message);

private:
    WinApp() = default;
    ~WinApp() = default;

    /// @brief ウィンドウクラス
    WNDCLASS wc_{};
    /// @brief ウィンドウハンドル
    HWND hwnd_{};
    /// @brief クライアントサイズの横幅
    int32_t kClientWidth_;
    /// @brief クライアントサイズの縦幅
    int32_t kClientHeight_;
    /// @brief ウィンドウサイズを表す構造体
    RECT wrc_;
    /// @brief メッセージ
    MSG msg_;

    /// @brief ウィンドウ初期化
    void InitializeWindow();

    /// @brief ウィンドウプロシージャ
    /// @param hwnd ウィンドウハンドル
    /// @param msg メッセージ
    /// @param wparam 
    /// @param lparam 
    /// @return メッセージ処理結果
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};