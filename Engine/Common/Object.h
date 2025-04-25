#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Math/Transform.h"

namespace MyEngine {

struct Object {
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

    Transform transform;
};

} // namespace MyEngine