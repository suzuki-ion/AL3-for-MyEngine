#pragma once
#include "Object.h"

namespace KashipanEngine {

struct Plane : public Object {
public:
    Plane();

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param transform 変形用のTransform
    void Draw(const Transform &transform);
};

} // namespace KashipanEngine