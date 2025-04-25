#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "VertexPosColor.h"

namespace MyEngine {

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

} // namespace MyEngine