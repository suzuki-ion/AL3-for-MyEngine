#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace MyEngine {

// 頂点データ構造体
struct VertexPosColor {
    Vector3 pos;    // xyz座標 
    Vector4 color;  // RGBA
};

} // namespace MyEngine