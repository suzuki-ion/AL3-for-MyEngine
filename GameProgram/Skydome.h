#pragma once
#include "Math/Transform.h"
#include "Math/Camera.h"
#include "Objects.h"

class Skydome {
public:
	// 初期化
	Skydome(KashipanEngine::Model* model);

	// 更新
	void Update();

	// 描画
	void Draw();

	// デバッグカメラ切り替え
	void ToggleDebugCamera() { isDebugCameraActive_ = !isDebugCameraActive_; }

private:
	// ワールド変換データ
	KashipanEngine::Transform worldTransform_;
	// モデル
	KashipanEngine::Model* model_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
};
