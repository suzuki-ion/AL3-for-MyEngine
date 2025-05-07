#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace MyEngine {

struct VertexData {
    Vector4 position;
    Vector2 texCoord;
    Vector3 normal;
};

} // namespace MyEngine