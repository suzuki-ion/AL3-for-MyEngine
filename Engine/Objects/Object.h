#pragma once
#include "Math/Transform.h"
#include "Math/Matrix4x4.h"
#include "Common/VertexData.h"
#include "Common/TransformationMatrix.h"
#include "Common/Material.h"
#include "3d/PrimitiveDrawer.h"

namespace MyEngine {

// 前方宣言
class Camera;

struct Object {
    Object() {
        // マテリアルリソースのマップを取得
        materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialMap));
        // TransformationMatrixリソースのマップを取得
        transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixMap));
    }
    ~Object() {
        // マテリアルリソースのアンマップ
        materialResource->Unmap(0, nullptr);
        // TransformationMatrixリソースのアンマップ
        transformationMatrixResource->Unmap(0, nullptr);
    }

    /// @brief カメラへのポインタ
    Camera *camera = nullptr;

    /// @brief マテリアル用のリソース
    const Microsoft::WRL::ComPtr<ID3D12Resource> materialResource =
        PrimitiveDrawer::CreateBufferResources(sizeof(Material));
    /// @brief TransformationMatrix用のリソース
    const Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource =
        PrimitiveDrawer::CreateBufferResources(sizeof(TransformationMatrix));

    /// @brief マテリアルマップ
    Material *materialMap = nullptr;
    /// @brief TransformationMatrixマップ
    TransformationMatrix *transformationMatrixMap = nullptr;

    /// @brief 変形用のtransform
    Transform transform;
    /// @brief ワールド行列
    Matrix4x4 worldMatrix;
    /// @brief マテリアルデータ
    Material material;

    /// @brief 使用するテクスチャのインデックス
    int useTextureIndex = -1;
};

} // namespace MyEngine