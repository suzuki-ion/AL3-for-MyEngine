#pragma once
#include "Object.h"

namespace KashipanEngine {

class Triangle : public Object {
public:
    Triangle();

    /// @brief 描画処理
    void Draw();

    /// @brief 描画処理
    /// @param transform 変形用のTransform
    void Draw(const Transform &transform);
};

} // namespace KashipanEngine