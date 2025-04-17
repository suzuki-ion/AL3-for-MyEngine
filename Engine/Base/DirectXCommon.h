#pragma once
#include <wrl.h>
#include <chrono>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#pragma comment(lib, "dxguid.lib")

namespace MyEngine {

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
    /// @param enableDebugLayer デバッグレイヤーを有効にするかどうか
    void Initialize(bool enableDebugLayer);
    /// @brief DirectX終了処理
    void Finalize();

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();

    /// @brief レンダーターゲットのクリア
    void ClearRenderTarget();

    /// @brief ディスクリプタヒープの作成
    /// @param heapType シープの種類
    /// @param numDescriptors ディスクリプタの数
    /// @param shaderVisible シェーダーから見えるかどうか
    /// @return ディスクリプタヒープのポインタ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible = false);

    /// @brief D3D12デバイス取得
    /// @return D3D12デバイス
    ID3D12Device *GetDevice() const { return device_.Get(); }

    /// @brief 描画コマンドリスト取得
    /// @return 描画コマンドリスト
    ID3D12GraphicsCommandList *GetCommandList() const { return commandList_.Get(); }

    /// @brief スワップチェインの設定取得
    /// @return スワップチェインの設定
    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc_; }

    /// @brief RTVの設定取得
    /// @return RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc_; }
    
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

    /// @brief スワップチェインの設定
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
    /// @brief スワップチェイン
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    /// @brief スワップチェインから取得したリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];

    //--------- レンダーターゲット ---------//

    /// @brief RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
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

} // namespace MyEngine