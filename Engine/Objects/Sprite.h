#pragma once
#include "Object.h"

namespace MyEngine {

struct Sprite : public Object {
    /// @brief 頂点データ
    VertexData vertexData[4];
};

} // namespace MyEngine