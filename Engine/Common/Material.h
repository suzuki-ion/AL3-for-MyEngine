#pragma once
#include <cstdint>
#include "Math/Vector4.h"

namespace MyEngine {

struct Material {
    /// @brief マテリアルの色
    Vector4 color;
    /// @brief Lightingの有効無効
    int32_t enableLighting;
    /// @brief padding分のメモリ
    float padding[3];
    /// @brief テクスチャ用のUVTransform行列
    Matrix4x4 uvTransform;
};

} // namespace MyEngine