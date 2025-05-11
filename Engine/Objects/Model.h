#pragma once
#include <vector>
#include <string>
#include "ModelData.h"
#include "Common/VertexData.h"

namespace MyEngine {

// 前方宣言
class TextureManager;

struct Model {
    /// @brief 必ずモデルのデータを設定してもらいたいので、デフォルトコンストラクタは削除
    Model() = delete;
    /// @brief Modelのコンストラクタ
    /// @param directoryPath モデルのディレクトリパス
    /// @param fileName モデルのファイル名
    /// @param textureManager テクスチャ管理クラスへのポインタ
    Model(std::string directoryPath, std::string fileName, TextureManager *textureManager);

    /// @brief モデルデータ
    std::vector<ModelData> models;
};

} // namespace MyEngine