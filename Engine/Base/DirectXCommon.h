#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

/// @brief Windowsアプリクラスの前方宣言
class WinApp;

/// @brief DirectX共通クラス
class DirectXCommon {
public:
    DirectXCommon(const DirectXCommon &) = delete;
    DirectXCommon &operator=(const DirectXCommon &) = delete;
    DirectXCommon(DirectXCommon &&) = delete;
    DirectXCommon &operator=(DirectXCommon &&) = delete;

    /// @brief インスタンス取得
    /// @return DirectXCommonクラスのインスタンス
    static DirectXCommon *GetInstance() {
        static DirectXCommon instance;
        return &instance;
    }

    /// @brief DirectX初期化
    void Initialize();
    
private:
    DirectXCommon() = default;
    ~DirectXCommon() = default;

    /// @brief Windowsアプリクラス
    WinApp *winApp_;
    
    /// @brief DXGIファクトリー
    IDXGIFactory7 *dxgiFactory_;
    /// @brief 使用するアダプタ
    IDXGIAdapter4 *useAdapter_;
    /// @brief D3D12デバイス
    ID3D12Device *device_;
    /// @brief コマンドキュー
    ID3D12CommandQueue *commandQueue_;

    /// @brief DXGI初期化
    void InitializeDXGI();
    /// @brief DXGIアダプター初期化
    void InitializeDXGIAdapter();
    /// @brief D3D12デバイス初期化
    void InitializeD3D12Device();
    /// @brief コマンドキュー初期化
    void InitializeCommandQueue();
};