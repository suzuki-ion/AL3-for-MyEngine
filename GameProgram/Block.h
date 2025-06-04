#pragma once
#include "Objects.h"
#include <memory>
#include <vector>

// 前方宣言
class MapChipField;

class Block {
public:
	/// @brief 初期化
	/// @param model モデル
	Block(KashipanEngine::Model* model);
	/// @brief 終了処理
	~Block();

	/// @brief マップチップフィールドのブロックを生成
	/// @param mapChipField マップチップフィールド
	void SetMapChipFieldBlocks(MapChipField* mapChipField);

	// 更新
	void Update();
	// 描画
	void Draw();

	// デバッグカメラ切り替え
	void ToggleDebugCamera() { isDebugCameraActive_ = !isDebugCameraActive_; }

private:
    void ResetWorldTransforms();

	// ワールド変換データ
	std::vector<std::vector<KashipanEngine::WorldTransform*>> worldTransforms_;
	// モデル
	KashipanEngine::Model* model_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
};
