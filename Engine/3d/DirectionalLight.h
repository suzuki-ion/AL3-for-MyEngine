#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"

namespace MyEngine {

struct DirectionalLight {
    /// @brief ライトの色
    Vector4 color;
    /// @brief ライトの向き
    Vector3 direction;
    /// @brief 輝度
    float intensity;
    /// @brief ライトのビューと射影行列
    Matrix4x4 viewProjectionMatrix;
};

} // namespace MyEngine