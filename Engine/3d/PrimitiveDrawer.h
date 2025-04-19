#pragma once
#include <Windows.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

namespace MyEngine {

/// @brief プリミティブ描画クラス
class PrimitiveDrawer final {
public:
    PrimitiveDrawer(const PrimitiveDrawer &) = delete;
    PrimitiveDrawer &operator=(const PrimitiveDrawer &) = delete;
    PrimitiveDrawer(PrimitiveDrawer &&) = delete;
    PrimitiveDrawer &operator=(PrimitiveDrawer &&) = delete;

    /// @brief インスタンス取得
    /// @return PrimitiveDrawerクラスのインスタンス
    static PrimitiveDrawer *GetInstance() {
        static PrimitiveDrawer instance;
        return &instance;
    }

    // 頂点データ構造体
    struct VertexPosColor {
        Vector3 pos;    // xyz座標 
        Vector4 color;  // RGBA
    };

    // パイプラインセット
    struct PipeLineSet {
        // ルートシグネチャ
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        // パイプラインステート
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    };

    // メッシュ
    struct Mesh {
        // 頂点バッファ
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
        // インデックスバッファ
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
        // 頂点バッファビュー
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
        // インデックスバッファビュー
        D3D12_INDEX_BUFFER_VIEW indexBufferView{};
        // 頂点バッファマップ
        VertexPosColor *vertexBufferMap = nullptr;
        // インデックスバッファマップ
        UINT16 *indexBufferMap = nullptr;
    };

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
    PrimitiveDrawer() = default;
    ~PrimitiveDrawer() = default;

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