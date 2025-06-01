#include "GameScene.h"
#include "Base/Input.h"

using namespace KashipanEngine;

// 要素数
extern const uint32_t kNumBlockVertical = 20;
extern const uint32_t kNumBlockHorizontal = 100;
// ブロック1個分の横幅
extern const float kBlockWidth = 2.0f;
// ブロック1個分の縦幅
extern const float kBlockHeight = 2.0f;

namespace {
// KashipanEngine
std::unique_ptr<Engine> sKashipanEngine;
} // namespace


GameScene::GameScene() {
	// KashipanEngineの初期化
    sKashipanEngine = std::make_unique<Engine>("Kashipan Engine Game", 1280, 720, true);

	// カメラの初期化
    camera_ = std::make_unique<Camera>();

	// プレイヤーの3Dモデルデータの生成
	modelPlayer_ = std::make_unique<Model>("Resource/Player", "player.obj");
	// ブロックの3Dモデルデータの生成
	modelBlock_ = std::make_unique<Model>("Resource/Block", "block.obj");
	// スカイドームの3Dモデルデータの生成
	modelSkydome_ = std::make_unique<Model>("Resource/Skydome", "skydome.obj");
	// マップチップフィールドの生成
    mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 自キャラの生成
    player_ = std::make_unique<Player>(modelPlayer_.get(), mapChipField_->GetMapChipPosition(1, kNumBlockVertical - 2));
	player_->SetMapChipField(mapChipField_.get());

	// ブロックの生成
    block_ = std::make_unique<Block>(modelBlock_.get());

	// スカイドームの生成
    skydome_ = std::make_unique<Skydome>(modelSkydome_.get());

	// カメラコントローラーの初期化
	cameraController_.SetCamera(&camera_);
	cameraController_.SetTarget(player_);

	// マップチップフィールドのブロックを生成
	block_->SetMapChipFieldBlocks(mapChipField_);
}

GameScene::~GameScene() {
}

void GameScene::GameLoop() {// ウィンドウのxボタンが押されるまでループ
    while (sKashipanEngine->ProccessMessage() != -1) {
        sKashipanEngine->BeginFrame();
        if (sKashipanEngine->BeginGameLoop(60) == false) {
            continue;
        }
        Input::Update();
        // ESCで終了
        if (Input::IsKeyTrigger(DIK_ESCAPE)) {
            break;
        }
    }
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	// ブロックの更新
	block_->Update();

	// カメラコントローラーの更新
	cameraController_.Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
		player_->ToggleDebugCamera();
		block_->ToggleDebugCamera();
		skydome_->ToggleDebugCamera();
	}

	// F5キーでマップチップフィールドをホットリロード
	if (Input::GetInstance()->TriggerKey(DIK_F5)) {
		mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
		block_->SetMapChipFieldBlocks(mapChipField_);
	}

	// Rキーでプレイヤーの位置をリセット
	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		player_->SetPosition(mapChipField_->GetMapChipPosition(1, kNumBlockVertical - 2));
	}

#endif // _DEBUG

	// デバッグカメラの更新
	if (isDebugCameraActive_) {
		camera_.matView = debugCameraViewMatrix_;
		camera_.matProjection = debugCameraProjectionMatrix_;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}
	debugCamera_->Update();
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画
	player_->Draw();

	// ブロックの描画
	block_->Draw();

	// スカイドームの描画
	skydome_->Draw();

	Model::PostDraw();
}

void GameScene::GenerateBlocks() {

}
