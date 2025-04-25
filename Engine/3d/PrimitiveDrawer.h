#pragma once
#include <Windows.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

#include "Common/VertexPosColor.h"

namespace MyEngine {

// 前方宣言
class WinApp;
class DirectXCommon;

/// @brief プリミティブ描画クラス
class PrimitiveDrawer final {
public:
    

    

    

    /// @brief PrimitiveDrawer初期化
    void Initialize();

    /// @brief PrimitiveDrawer終了処理
    void Finalize();

    /// @brief リソース生成
    /// @param size サイズ
    /// @return 生成したリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResources(UINT64 size);

    /// @brief メッシュ生成
    /// @param vertexCount 頂点数
    /// @param indexCount インデックス数
    /// @return 生成したメッシュ
    std::unique_ptr<Mesh> CreateMesh(UINT vertexCount);
    
    /// @brief グラフィックパイプライン生成
    /// @param topologyType トポロジータイプ
    /// @return パイプラインセット
    std::unique_ptr<PipeLineSet> CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);

    /// @brief リセット
    void Reset();

private:
    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;

    /// @brief メッシュ
    std::unique_ptr<Mesh> mesh_;
    /// @brief パイプラインセット
    std::unique_ptr<PipeLineSet> pipelineSet_;

    /// @brief グラフィックパイプライン初期化
    void InitializeGraphicsPipeline();
    /// @brief メッシュ初期化
    void InitializeMesh();
};

} // namespace MyEngine