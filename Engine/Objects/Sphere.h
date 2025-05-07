#pragma once
#include "Object.h"

namespace MyEngine {

struct Sphere : public Object {
    /// @brief 必ず分割数を設定してもらいたいのでデフォルトコンストラクタは削除
    Sphere() = delete;
    /// @brief 球体のコンストラクタ
    /// @param subdivision 分割数
    Sphere(const int subdivision) :
        kSubdivision(subdivision),
        kVertexCount((subdivision) * (subdivision) * 6)
    {
        // メッシュの生成
        mesh = PrimitiveDrawer::CreateMesh(kVertexCount);
    }

    /// @brief 初期化用関数
    void Initialize();

    /// @brief 半径
    float radius;
    /// @brief 描画時の分割数
    const uint32_t kSubdivision;
    /// @brief 頂点数
    const uint32_t kVertexCount;
};

} // namespace MyEngine