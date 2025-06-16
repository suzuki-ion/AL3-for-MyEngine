#include "AxisIndicator.h"
#include "Math/Camera.h"
#include "Objects/Model.h"

namespace KashipanEngine {

namespace {
// 軸のモデル
std::unique_ptr<Model> axisModel = nullptr;
} // namespace

void AxisIndicator::Initialize() {
    if (axisModel) return; // 既に初期化済みなら何もしない
    // 軸のモデルを生成
    axisModel = std::make_unique<Model>("Resources/Axis", "axis.obj");
}

void AxisIndicator::Finalize() {
    axisModel.reset();
}

void AxisIndicator::Draw(Camera *camera) {
}

} // namespace KashipanEngine