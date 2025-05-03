#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <source_location>

namespace MyEngine {

// 前方宣言
class DirectXCommon;

/*
シングルトンでどこからでもアクセスできてしまうため、
必ずどこから呼び出されたかを特定するために引数にsource_locationを設定
*/

/// @brief シェーダリソースビュー用クラス
class SRV {
public:
    SRV(const SRV &) = delete;
    SRV(const SRV &&) = delete;
    SRV &operator=(const SRV &) = delete;
    SRV &operator=(const SRV &&) = delete;

    /// @brief 初期化処理
    /// @param winApp WinAppインスタンスへのポインタ
    /// @param dxCommon DirectXCommonインスタンスへのポインタ
    static void Initialize(
        DirectXCommon *dxCommon,
        const std::source_location &location = std::source_location::current()
    );

    /// @brief 終了処理
    static void Finalize(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief DescriptorHeapの取得
    /// @return DescriptorHeapのポインタ
    [[nodiscard]] static ID3D12DescriptorHeap *GetDescriptorHeap(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief CPUディスクリプタハンドルの取得 
    /// @return CPUディスクリプタハンドル
    [[nodiscard]] static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
        const std::source_location &location = std::source_location::current()
    );

    /// @brief GPUディスクリプタハンドルの取得
    /// @return GPUディスクリプタハンドル
    [[nodiscard]] static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
        const std::source_location &location = std::source_location::current()
    );

private:
    SRV() = default;
    ~SRV() = default;

    /// @brief 初期化フラグ
    static bool isInitialized_;
    /// @brief DirectXCommonインスタンス
    static DirectXCommon *dxCommon_;

    /// @brief ディスクリプタヒープの数
    static const uint32_t numDescriptors_ = 128;
    /// @brief ディスクリプタヒープ
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    /// @brief 次に使用するCPUディスクリプタハンドルのインデックス
    static uint32_t nextIndexCPU_;
    /// @brief 次に使用するGPUディスクリプタハンドルのインデックス
    static uint32_t nextIndexGPU_;
};

} // namespace MyEngine