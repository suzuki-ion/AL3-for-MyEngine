#pragma once
#include <cstdint>
#include "Math/Vector4.h"

namespace MyEngine {

struct Material {
    /// @brief マテリアルの色
    Vector4 color;
    /// @brief Lightingの有効無効
    int32_t enableLighting;
};

} // namespace MyEngine