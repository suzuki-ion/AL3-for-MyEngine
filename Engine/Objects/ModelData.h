#pragma once
#include <vector>
#include <string>
#include "Object.h"
#include "Common/VertexData.h"
#include "Objects/MaterialData.h"

namespace MyEngine {

// 前方宣言
class TextureManager;

struct ModelData : public Object {
    /// @brief モデルのマテリアルデータ
    struct MaterialData {
        std::string textureFilePath;
    };

    /// @brief 必ずモデルのデータを設定してもらいたいので、デフォルトコンストラクタは削除
    ModelData() = delete;
    /// @brief ModelDataのコンストラクタ
    /// @param directoryPath モデルのディレクトリパス
    /// @param fileName モデルのファイル名
    /// @param textureManager テクスチャ管理クラスへのポインタ
    ModelData(std::string directoryPath, std::string fileName, TextureManager *textureManager);

    /// @brief モデルの頂点データ
    std::vector<VertexData> vertices;
    /// @brief モデルのマテリアル
    MaterialData materialData;
};

} // namespace MyEngine