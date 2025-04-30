#pragma once
#include "Object.h"

namespace MyEngine {

struct Triangle : public Object {
    /// @brief 頂点データ
    VertexData vertexData[3];
};

} // namespace MyEngine