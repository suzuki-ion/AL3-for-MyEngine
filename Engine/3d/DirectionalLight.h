#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace MyEngine {

struct DirectionalLight {
    /// @brief ライトの色
    Vector4 color;
    /// @brief ライトの向き
    Vector3 direction;
    /// @brief 輝度
    float intensity;
};

} // namespace MyEngine