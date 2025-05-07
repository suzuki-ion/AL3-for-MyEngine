#pragma once
#include "Object.h"

namespace MyEngine {

struct Sprite : public Object {
    Sprite() {
        // メッシュの生成
        mesh = PrimitiveDrawer::CreateMesh(6);
    }

    /// @brief 頂点データ
    VertexData vertexData[4];
};

} // namespace MyEngine