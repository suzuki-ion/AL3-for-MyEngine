#pragma once
#include "Object.h"

namespace MyEngine {

struct BillBoard : public Object {
    /// @brief 必ずカメラを設定してもらいたいのでデフォルトコンストラクタは削除
    BillBoard() = delete;
    /// @brief ビルボードのコンストラクタ
    /// @param camera カメラ
    BillBoard(Camera *camera) {
        this->camera = camera;
        mesh = PrimitiveDrawer::CreateMesh(6);
    }

    /// @brief 頂点データ
    VertexData vertexData[4];
};

} // namespace MyEngine