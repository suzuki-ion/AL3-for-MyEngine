#pragma once
#include "KashipanEngine.h"
#include "Objects.h"
#include "Math/Camera.h"
#include "Player.h"
#include "Block.h"
#include "Skydome.h"
#include "CameraController.h"
#include "MapChipField.h"
#include <vector>

class GameScene {
public:
    // コンストラクタ
    GameScene();
    // デストラクタ
    ~GameScene();

	// ゲームループ
    void GameLoop();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// マップチップに合わせてブロックを生成
	void GenerateBlocks();

	// プレイヤーの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelPlayer_;
	// ブロックの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelBlock_;
	// スカイドームの3Dモデルデータ
	std::unique_ptr<KashipanEngine::Model> modelSkydome_;

	// カメラコントローラー
    std::unique_ptr<CameraController> cameraController_;
	// マップチップフィールド
    std::unique_ptr<MapChipField> mapChipField_;

	// カメラ
	std::unique_ptr<KashipanEngine::Camera> camera_;
	// デバッグカメラ有効フラグ
	bool isDebugCameraActive_ = false;

	// 自キャラ
    std::unique_ptr<Player> player_;
	// ブロック
    std::unique_ptr<Block> block_;
	// スカイドーム
    std::unique_ptr<Skydome> skydome_;
};
