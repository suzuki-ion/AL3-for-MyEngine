#pragma once
#include "Math/Transform.h"
#include "Math/Matrix4x4.h"
#include "Common/VertexData.h"
#include "3d/PrimitiveDrawer.h"

namespace MyEngine {

// 前方宣言
class Camera;

struct Object {
    /// @brief カメラへのポインタ
    Camera *camera = nullptr;

    /// @brief メッシュ
    std::unique_ptr<Mesh> mesh = nullptr;
    /// @brief マテリアル用のリソース
    const Microsoft::WRL::ComPtr<ID3D12Resource> materialResource =
        PrimitiveDrawer::CreateBufferResources(sizeof(Vector4));
    /// @brief WVP用のリソース
    const Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource =
        PrimitiveDrawer::CreateBufferResources(sizeof(Matrix4x4));

    /// @brief 変形用のtransform
    Transform transform;
    /// @brief ワールド行列
    Matrix4x4 worldMatrix;
    /// @brief 色データ
    Vector4 color;

    /// @brief 使用するテクスチャのインデックス
    int useTextureIndex = -1;
};

} // namespace MyEngine