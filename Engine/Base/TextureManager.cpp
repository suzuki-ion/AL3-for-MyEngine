#include "TextureManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "2d/ImGuiManager.h"
#include "3d/PrimitiveDrawer.h"
#include "Common/Logs.h"
#include "Common/ConvertString.h"

namespace MyEngine {

namespace {

/// @brief テクスチャファイルを読み込んで扱えるようにする
/// @param filePath テクスチャファイルのパス
/// @return 読み込んだテクスチャのScratchImage
DirectX::ScratchImage LoadTexture(const std::string &filePath) {
    // テクスチャファイルを読み込んで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(
        filePathW.c_str(),
        DirectX::WIC_FLAGS_FORCE_SRGB,
        nullptr,
        image
    );
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(
        image.GetImages(),
        image.GetImageCount(),
        image.GetMetadata(),
        DirectX::TEX_FILTER_SRGB,
        0,
        mipImages
    );
    assert(SUCCEEDED(hr));
    
    // ミップマップ付きのデータを返す
    return mipImages;
}

} // namespace

TextureManager::TextureManager(WinApp *winApp, DirectXCommon *dxCommon, PrimitiveDrawer *primitiveDrawer, ImGuiManager *imguiManager) {
    // nullチェック
    if (winApp == nullptr) {
        Log("winApp is null.", true);
        assert(false);
    }
    if (dxCommon == nullptr) {
        Log("dxCommon is null.", true);
        assert(false);
    }
    if (primitiveDrawer == nullptr) {
        Log("primitiveDrawer is null.", true);
        assert(false);
    }
    if (imguiManager == nullptr) {
        Log("imguiManager is null.", true);
        assert(false);
    }
    // 引数をメンバ変数に格納
    winApp_ = winApp;
    dxCommon_ = dxCommon;
    primitiveDrawer_ = primitiveDrawer;
    imguiManager_ = imguiManager;

    // 初期化完了のログを出力
    Log("Initialized.");
}

uint32_t TextureManager::Load(const std::string &filePath) {
    // テクスチャファイルを読み込んで扱えるようにする
    DirectX::ScratchImage mipImages = LoadTexture(filePath);
    // ミップマップのメタデータを取得
    const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
    // テクスチャリソースを作成
    CreateTextureResource(metadata);

    // テクスチャデータをアップロード
    intermediateResources_.push_back(UploadTextureData(textureResource_.Get(), mipImages));
    // コマンドを実行
    dxCommon_->CommandExecute();

    // metadataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVを作成するDescriptorHeapの場所を決める
    textureSrvHandleCPU_=
        imguiManager_->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    textureSrvHandleGPU_ =
        imguiManager_->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

    // 先頭はImGuiが使っているのでその次を使う
    textureSrvHandleCPU_.ptr +=
        dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    textureSrvHandleGPU_.ptr +=
        dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // SRVの生成
    dxCommon_->GetDevice()->CreateShaderResourceView(
        textureResource_.Get(),
        &srvDesc,
        textureSrvHandleCPU_
    );

    // Barrierを元に戻す
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = textureResource_.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    dxCommon_->SetBarrier(barrier);

    // 読み込んだテクスチャのログを出力
    Log(std::format("Load Texture: {}", filePath));

    // テクスチャのハンドルを返す
    return static_cast<uint32_t>(intermediateResources_.size() - 1);
}

void TextureManager::CreateTextureResource(const DirectX::TexMetadata &metadata) {
    //==================================================
    // metadataを基にResourceの設定
    //==================================================

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);                              // Textureの幅
    resourceDesc.Height = UINT(metadata.height);                            // Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels);                    // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);             // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format;                                  // TextureのFormat
    resourceDesc.SampleDesc.Count = 1;                                      // サンプリングカウント
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);  // Textureの次元数

    //==================================================
    // 利用するHeapの設定
    //==================================================

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //==================================================
    // Resourceを生成する
    //==================================================

    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProperties,                    // Heapの設定
        D3D12_HEAP_FLAG_NONE,               // Heapの特殊な設定
        &resourceDesc,                      // Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST,     // データ転送される設定
        nullptr,                            // Clear最適値。使わないのでnullptr
        IID_PPV_ARGS(&textureResource_)     // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages) {
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(
        dxCommon_->GetDevice(),
        mipImages.GetImages(),
        mipImages.GetImageCount(),
        mipImages.GetMetadata(),
        subresources
    );
    uint64_t intermediateSize = GetRequiredIntermediateSize(
        texture,
        0,
        UINT(subresources.size())
    );
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =
        primitiveDrawer_->CreateBufferResources(intermediateSize);
    // データ転送をコマンドに積む
    UpdateSubresources(
        dxCommon_->GetCommandList(),
        texture,
        intermediateResource.Get(),
        0,
        0,
        UINT(subresources.size()),
        subresources.data()
    );

    // Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEに遷移
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    dxCommon_->SetBarrier(barrier);

    return intermediateResource;
}

} // namespace MyEngine
