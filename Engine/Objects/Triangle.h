#pragma once
#include "Object.h"

namespace MyEngine {

struct Triangle : public Object {
    Triangle() :
        mesh(PrimitiveDrawer::CreateMesh(3, 3))
    {
        mesh->indexBufferMap[0] = 0;
        mesh->indexBufferMap[1] = 1;
        mesh->indexBufferMap[2] = 2;
    }

    /// @brief メッシュ
    const std::unique_ptr<Mesh> mesh;
};

} // namespace MyEngine