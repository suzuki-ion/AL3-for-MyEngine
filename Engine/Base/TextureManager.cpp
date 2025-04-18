#include "TextureManager.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "2d/ImGuiManager.h"
#include "ConvertString.h"

namespace MyEngine {

namespace {
// 各エンジン用クラスのグローバル変数
WinApp *sWinApp = nullptr;
DirectXCommon *sDxCommon = nullptr;
ImGuiManager *sImGuiManager = nullptr;
} // namespace

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

void TextureManager::Initialize() {
    // 各エンジン用クラスのインスタンスを取得
    sWinApp = WinApp::GetInstance();
    sDxCommon = DirectXCommon::GetInstance();
    sImGuiManager = ImGuiManager::GetInstance();

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize TextureManager.");
}

void TextureManager::Finalize() {
}

uint32_t TextureManager::Load(const std::string &filePath) {
    // テクスチャファイルを読み込んで扱えるようにする
    DirectX::ScratchImage mipImages = LoadTexture(filePath);
    // ミップマップのメタデータを取得
    const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
    // テクスチャリソースを作成
    CreateTextureResource(metadata);
    // テクスチャデータをアップロード
    UploadTextureData(textureResource_.Get(), mipImages);

    // metadataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVを作成するDescriptorHeapの場所を決める
    textureSrvHandleCPU_=
        sImGuiManager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    textureSrvHandleGPU_ =
        sImGuiManager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

    // 先頭はImGuiが使っているのでその次を使う
    textureSrvHandleCPU_.ptr +=
        sDxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    textureSrvHandleGPU_.ptr +=
        sDxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // SRVの生成
    sDxCommon->GetDevice()->CreateShaderResourceView(
        textureResource_.Get(),
        &srvDesc,
        textureSrvHandleCPU_
    );

    // 読み込んだテクスチャのログを出力
    sWinApp->Log(std::format("Load Texture: {}", filePath));

    // テクスチャのハンドルを返す
    return 0;
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
    heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;                           // 細かい設定を行う
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;    // WhiteBackポリシーでCPUアクセス可能
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;             // プロセッサの近くに配置

    //==================================================
    // Resourceを生成する
    //==================================================

    HRESULT hr = sDxCommon->GetDevice()->CreateCommittedResource(
        &heapProperties,                    // Heapの設定
        D3D12_HEAP_FLAG_NONE,               // Heapの特殊な設定
        &resourceDesc,                      // Resourceの設定
        D3D12_RESOURCE_STATE_GENERIC_READ,  // 初回のResourceState。Textureは基本読むだけ
        nullptr,                            // Clear最適値。使わないのでnullptr
        IID_PPV_ARGS(&textureResource_)     // 作成するResourceポインタへのポインタ
    );
    assert(SUCCEEDED(hr));
}

void TextureManager::UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages) {
    // Meta情報を取得
    const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
    
    // 全MipMapについて
    for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
        // MipMapLevelを指定して全Imageを取得
        const DirectX::Image *img = mipImages.GetImage(mipLevel, 0, 0);
        // Textureに転送
        HRESULT hr = texture->WriteToSubresource(
            UINT(mipLevel),
            nullptr,                // 全領域へコピー
            img->pixels,            // 元データアドレス
            UINT(img->rowPitch),    // 1ラインサイズ
            UINT(img->slicePitch)   // 1枚サイズ
        );
        assert(SUCCEEDED(hr));
    }
}

} // namespace MyEngine
