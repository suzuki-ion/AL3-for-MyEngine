#pragma once
#include <memory>
#include <d3d12.h>
#include <wrl.h>

#include "Math/Transform.h"
#include "Math/Matrix4x4.h"
#include "Common/VertexData.h"
#include "Common/Mesh.h"

namespace MyEngine {

// 前方宣言
class Camera;

struct Object {
    /// @brief カメラへのポインタ
    Camera *camera = nullptr;

    /// @brief メッシュ
    std::unique_ptr<Mesh> mesh = nullptr;
    /// @brief マテリアル用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
    /// @brief WVP用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;

    /// @brief 変形用のtransform
    Transform transform;
    /// @brief ワールド行列
    Matrix4x4 worldMatrix;
    /// @brief 色データ
    Vector4 color;
};

} // namespace MyEngine