#pragma once
#include <cstdint>
#include "Math/Vector4.h"

namespace MyEngine {

struct Material {
    /// @brief マテリアルの色
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief Lightingの有効無効
    int32_t enableLighting = true;
    /// @brief padding分のメモリ
    float padding[3];
    /// @brief テクスチャ用のUVTransform行列
    Matrix4x4 uvTransform;
    /// @brief 拡散光の色
    Vector4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief 反射光の色
    Vector4 specularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief 環境光の色
    Vector4 ambientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// @brief 放射光の色
    Vector4 emissiveColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

} // namespace MyEngine