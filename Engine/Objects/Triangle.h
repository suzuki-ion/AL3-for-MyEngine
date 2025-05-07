#pragma once
#include "Object.h"

namespace MyEngine {

struct Triangle : public Object {
    Triangle() {
        // メッシュの生成
        mesh = PrimitiveDrawer::CreateMesh(3);
    }

    /// @brief 頂点データ
    VertexData vertexData[3];
};

} // namespace MyEngine