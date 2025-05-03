#pragma once
#include <vector>
#include <memory>

#include "Common/VertexData.h"
#include "Common/Mesh.h"
#include "Common/PipeLineSet.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"

namespace MyEngine {

// 前方宣言
class WinApp;
class DirectXCommon;
class ImGuiManager;
class TextureManager;

struct Triangle;
struct Sprite;
struct Sphere;
struct BillBoard;

/// @brief 描画用クラス
class Drawer {
public:
    /// @brief コンストラクタ
    /// @param winApp WinAppインスタンス
    /// @param dxCommon DirectXCommonインスタンス
    /// @param primitiveDrawer PrimitiveDrawerインスタンス
    /// @param imguiManager ImGuiManagerインスタンス
    Drawer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager, TextureManager *textureManager);

    /// @brief デストラクタ
    ~Drawer();

    /// @brief 描画開始前処理
    void PreDraw();

    /// @brief 描画終了処理
    void PostDraw();

    /// @brief 三角形を描画する
    /// @param triangle 描画する三角形へのポインタ
    void Draw(Triangle *triangle);

    /// @brief スプライトを描画する
    /// @param sprite 描画するスプライトへのポインタ
    void Draw(Sprite *sprite);

    /// @brief 球体を描画する
    /// @param sphere 描画する球体へのポインタ
    void Draw(Sphere *sphere);

    /// @brief ビルボードを描画する
    /// @param billboard 描画するビルボードへのポインタ
    void Draw(BillBoard *billboard);

private:
    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;
    /// @brief ImGuiManagerインスタンス
    ImGuiManager *imguiManager_ = nullptr;
    /// @brief TextureManagerインスタンス
    TextureManager *textureManager_ = nullptr;

    /// @brief 頂点データ設定用のポインタ
    VertexData *vertexData_ = nullptr;
    /// @brief マテリアルデータ設定用のポインタ
    Vector4 *materialData_ = nullptr;
    /// @brief WVP行列設定用のポインタ
    Matrix4x4 *wvpData_ = nullptr;

    /// @brief 2D描画用のビュー行列
    Matrix4x4 viewMatrix2D_;
    /// @brief 2D描画用のプロジェクション行列
    Matrix4x4 projectionMatrix2D_;
    /// @brief 2D描画用のWVP行列
    Matrix4x4 wvpMatrix2D_;
};

} // namespace MyEngine