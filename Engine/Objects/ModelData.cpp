#include <fstream>
#include <sstream>
#include <cassert>

#include "ModelData.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Common/Logs.h"
#include "Base/TextureManager.h"

namespace MyEngine {

namespace {

ModelData::MaterialData LoadMaterialTemplateFile(const std::string &directoryPath, const std::string &fileName) {
    ModelData::MaterialData materialData;  // 構築するMaterialData
    std::string line;           // ファイルから読み込んだ1行を格納するもの
    std::ifstream file(directoryPath + "/" + fileName); // ファイルを開く
    if (!file.is_open()) {
        Log("Failed to open file: " + directoryPath + "/" + fileName, kLogLevelFlagError);
        assert(false);
    }

    // ファイルを1行ずつ読み込む
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        // 先頭の識別子を読む
        s >> identifier;

        // identifierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFileName;
            s >> textureFileName;
            // 連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFileName;
        }
    }

    return materialData;
}

} // namespace

ModelData::ModelData(std::string directoryPath, std::string fileName, TextureManager *textureManager) {
    std::vector<Vector4> positions; // 位置
    std::vector<Vector3> normals;   // 法線
    std::vector<Vector2> texCoords; // テクスチャ座標
    std::vector<uint32_t> index;    // インデックスデータ
    std::string line;               // ファイルから読み込んだ1行を格納するもの

    // ファイルを開く
    std::ifstream file(directoryPath + "/" + fileName);
    if (!file.is_open()) {
        Log("Failed to open file: " + directoryPath + "/" + fileName, kLogLevelFlagError);
        assert(false);
    }

    // ファイルを1行ずつ読み込む
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        // 先頭の識別子を読む
        s >> identifier;

        // identifierに応じた処理
        if (identifier == "v") {
            //--------- 頂点読み込み ---------//

            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            // モデルは右手系なので左手系に変換
            position.x *= -1.0f;
            position.z *= -1.0f;
            positions.push_back(position);

        } else if (identifier == "vt") {
            //--------- テクスチャ座標読み込み ---------//

            Vector2 texCoord;
            s >> texCoord.x >> texCoord.y;
            // テクスチャ座標はY軸反転
            texCoord.y = 1.0f - texCoord.y;
            texCoords.push_back(texCoord);

        } else if (identifier == "vn") {
            //--------- 法線ベクトル読み込み ---------//

            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            // モデルは右手系なので左手系に変換
            normal.x *= -1.0f;
            normal.z *= -1.0f;
            normals.push_back(normal);

        } else if (identifier == "f") {
            //--------- 面の頂点データ読み込み ---------//

            std::vector<VertexData> faceVertices;
            while (true) {
                // 読み込む頂点が無くなったら終了
                if (s.eof()) {
                    break;
                }

                std::string vertexDefinition;
                s >> vertexDefinition;
                // 頂点の要素へのindexは「位置/UV/法線」で格納されているので、分解してindexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3] = { 1, 1, 1 };
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    // 区切りでインデックスを読んでいく
                    std::getline(v, index, '/');
                    // indexが空文字列の場合は、次の要素へ
                    if (index.empty()) {
                        continue;
                    }
                    elementIndices[element] = std::stoi(index);
                }
                // 要素へのindexから、実際の要素の値を取得して、頂点を構成する
                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texCoord = texCoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];
                faceVertices.push_back({ position, texCoord, normal });
            }
            for (size_t i = 0; i < faceVertices.size(); ++i) {
                vertices.push_back(faceVertices[i]);
            }
            // インデックスを設定する
            size_t indexOffset = vertices.size() - faceVertices.size();
            for (size_t i = 0; i <= faceVertices.size() - 3; ++i) {
                if (i % 2 == 0) {
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 0)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 1)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 2)));
                } else {
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 1)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 2)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i - 1)));
                }
            }

        } else if (identifier == "mtllib") {
            std::string materialFileName;
            s >> materialFileName;
            // 基本的にobjファイルと同一階層にmtlは存在するので、ディレクトリ名とファイル名を渡す
            materialData = LoadMaterialTemplateFile(directoryPath, materialFileName);
        }
    }

    // インデックスの数を設定
    indexCount = static_cast<UINT>(index.size());
    // メッシュの生成
    mesh = PrimitiveDrawer::CreateMesh(static_cast<UINT>(vertices.size()), indexCount);
    // メッシュの頂点バッファにデータをコピー
    std::memcpy(mesh->vertexBufferMap, vertices.data(), sizeof(VertexData) * vertices.size());
    // メッシュのインデックスバッファにデータをコピー
    std::memcpy(mesh->indexBufferMap, index.data(), sizeof(uint32_t) * index.size());

    // テクスチャの読み込み
    if (materialData.textureFilePath.empty()) {
        // テクスチャが指定されていない場合は0を指定
        useTextureIndex = 0;
    } else {
        useTextureIndex = textureManager->Load(materialData.textureFilePath);
    }
}

} // namespace MyEngine