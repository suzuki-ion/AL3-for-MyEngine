#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace MyEngine {

struct Texture {
    /// @brief テクスチャリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    /// @brief SRVハンドル(CPU)
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
    /// @brief SRVハンドル(GPU)
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
};

} // namespace MyEngine