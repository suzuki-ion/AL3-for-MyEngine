#pragma once
#include <chrono>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxgidebug.h>

#pragma comment(lib, "dxguid.lib")

/// @brief DirectX共通クラス
class DirectXCommon final {
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
    void Initialize(bool enableDebugLayer);
    /// @brief DirectX終了処理
    void Finalize();

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();

    /// @brief レンダーターゲットのクリア
    void ClearRenderTarget();
    
private:
    DirectXCommon() = default;
    ~DirectXCommon() = default;

    /// @brief リソースリークチェック用構造体
    struct D3DResourceLeakChecker {
        ~D3DResourceLeakChecker();
    };
    /// @brief リソースリークチェック用変数
    D3DResourceLeakChecker leakCheck_;

    //--------- DXGI ---------//
    
    /// @brief DXGIファクトリー
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    /// @brief 使用するアダプタ
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
    /// @brief D3D12デバイス
    Microsoft::WRL::ComPtr<ID3D12Device> device_;

    //--------- コマンド ---------//

    /// @brief コマンドキュー
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    /// @brief コマンドアロケータ
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    /// @brief コマンドリスト
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

    //--------- スワップチェイン ---------//

    /// @brief スワップチェイン
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    /// @brief スワップチェインから取得したリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];

    //--------- レンダーターゲット ---------//

    /// @brief RTVのディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
    /// @brief RTVのディスクリプタヒープのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_[2];

    //--------- フェンス ---------//

    /// @brief フェンス
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    /// @brief フェンスの値
    UINT64 fenceValue_;
    /// @brief フェンスのイベントハンドル
    HANDLE fenceEvent_;

    //--------- 各種初期化用関数 ---------//

    /// @brief DXGI初期化
    void InitializeDXGI();
    /// @brief DXGIアダプター初期化
    void InitializeDXGIAdapter();
    /// @brief D3D12デバイス初期化
    void InitializeD3D12Device();

    /// @brief コマンドキュー初期化
    void InitializeCommandQueue();
    /// @brief コマンドアロケータ初期化
    void InitializeCommandAllocator();
    /// @brief コマンドリスト初期化
    void InitializeCommandList();

    /// @brief スワップチェイン初期化
    void InitializeSwapChain();
    /// @brief RTVのディスクリプタヒープ初期化
    void InitializeRTVDescriptorHeap();
    /// @brief スワップチェインから取得したリソース初期化
    void InitializeSwapChainResources();
    /// @brief RTVのディスクリプタヒープのハンドル初期化
    void InitializeRTVHandle();

    /// @brief フェンス初期化
    void InitializeFence();
};