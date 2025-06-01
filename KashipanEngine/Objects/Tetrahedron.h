#pragma once
#include "Object.h"

namespace KashipanEngine {

class Tetrahedron : public Object {
public:
    Tetrahedron();

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param transform 変形用のTransform
    void Draw(const Transform &transform);
};

} // namespace KashipanEngine
