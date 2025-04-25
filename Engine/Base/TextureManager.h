#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <d3dx12.h>
#include <wrl.h>
#include <DirectXTex.h>

namespace MyEngine {

/// @brief テクスチャ管理クラス
class TextureManager {
public:
    TextureManager(const TextureManager &) = delete;
    TextureManager &operator=(const TextureManager &) = delete;
    TextureManager(TextureManager &&) = delete;
    TextureManager &operator=(TextureManager &&) = delete;
    
    struct Texture {
        /// @brief テクスチャリソース
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        /// @brief SRVハンドル(CPU)
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
        /// @brief SRVハンドル(GPU)
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
    };

    /// @brief インスタンス取得
    /// @return TextureManagerクラスのインスタンス
    static TextureManager *GetInstance() {
        static TextureManager instance;
        return &instance;
    }
    
    /// @brief テクスチャ初期化
    void Initialize();

    /// @brief テクスチャ終了処理
    void Finalize();

    /// @brief テクスチャの読み込み
    /// @param filePath 読み込むテクスチャのファイル名
    /// @return テクスチャのハンドル
    uint32_t Load(const std::string &filePath);

    /// @brief テクスチャのSRVハンドルを取得
    /// @return テクスチャのSRVハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU() const {
        return textureSrvHandleCPU_;
    }

    /// @brief テクスチャのSRVハンドルを取得
    /// @return テクスチャのSRVハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() const {
        return textureSrvHandleGPU_;
    }

private:
    TextureManager() = default;
    ~TextureManager() = default;

    /// @brief テクスチャリソースのハンドル
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
    /// @brief テクスチャリソースのハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
    /// @brief テクスチャリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
    /// @brief 中間リソース
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

    /// @brief テクスチャのリソースを作成
    /// @param metadata テクスチャのメタデータ
    void CreateTextureResource(const DirectX::TexMetadata &metadata);

    /// @brief TextureResourceにデータをアップロードする
    /// @param texture テクスチャリソース
    /// @param mipImages ミップマップ画像
    /// @return アップロードしたリソース
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages);
};

} // namespace MyEngine