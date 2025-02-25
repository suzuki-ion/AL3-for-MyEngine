#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>

class WinApp final {
public:
    /// @brief エンジン初期化
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

    /// @brief DXGIファクトリー
    IDXGIFactory7 *dxgiFactory_;
    /// @brief Windows系のエラーコード
    HRESULT hr_;

    /// @brief 使用するアダプタ
    IDXGIAdapter4 *useAdapter_;
    /// @brief D3D12デバイス
    ID3D12Device *device_;

    /// @brief ウィンドウ初期化
    void InitializeWindow();
    /// @brief DXGI初期化
    void InitializeDXGI();
    /// @brief DXGIアダプター初期化
    void InitializeDXGIAdapter();
    /// @brief D3D12デバイス初期化
    void InitializeD3D12Device();

    /// @brief ウィンドウプロシージャ
    /// @param hwnd ウィンドウハンドル
    /// @param msg メッセージ
    /// @param wparam 
    /// @param lparam 
    /// @return メッセージ処理結果
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};