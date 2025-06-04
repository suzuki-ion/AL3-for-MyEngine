#include <cassert>
#include "GameScene.h"
#include "Base/WinApp.h"
#include "Base/Input.h"
#include "Base/Renderer.h"

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
Engine *sKashipanEngine;
// WinApp
WinApp *sWinApp = nullptr;
// レンダラー
Renderer *sRenderer = nullptr;
} // namespace

GameScene::GameScene(Engine *engine) {
    // NULLポインタチェック
    assert(engine);
    // エンジンの設定
    sKashipanEngine = engine;

	// レンダラーの取得
	sRenderer = sKashipanEngine->GetRenderer();
    // WinAppの取得
    sWinApp = sKashipanEngine->GetWinApp();
	// 画面のサイズ変更を有効化
    sWinApp->SetSizeChangeMode(kNormal);

	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	// レンダラーにカメラを設定
	sRenderer->SetCamera(camera_.get());

	// プレイヤーの3Dモデルデータの生成
	modelPlayer_ = std::make_unique<Model>("Resources/Player", "player.obj");
	// ブロックの3Dモデルデータの生成
	modelBlock_ = std::make_unique<Model>("Resources/Block", "block.obj");
	// スカイドームの3Dモデルデータの生成
	modelSkydome_ = std::make_unique<Model>("Resources/Skydome", "skydome.obj");
	// レンダラーを設定
	modelPlayer_->SetRenderer(sRenderer);
	modelBlock_->SetRenderer(sRenderer);
	modelSkydome_->SetRenderer(sRenderer);

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

	// カメラコントローラーの生成
	cameraController_ = std::make_unique<CameraController>();
	// カメラコントローラーの初期化
	cameraController_->SetCamera(camera_.get());
	cameraController_->SetTarget(player_.get());

	// マップチップフィールドのブロックを生成
	block_->SetMapChipFieldBlocks(mapChipField_.get());
}

GameScene::~GameScene() {
}

void GameScene::GameLoop() {
	// ウィンドウのxボタンが押されるまでループ
    while (sKashipanEngine->ProccessMessage() != -1) {
        sKashipanEngine->BeginFrame();
        if (sKashipanEngine->BeginGameLoop(60) == false) {
            continue;
        }
        Input::Update();

        // 更新
        Update();
        // 描画
        Draw();

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

    // スカイドームの更新
    skydome_->Update();

	// カメラコントローラーの更新
	cameraController_->Update();

#ifdef _DEBUG
	if (Input::IsKeyTrigger(DIK_F1)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
        sRenderer->ToggleDebugCamera();
	}

	// F5キーでマップチップフィールドをホットリロード
	if (Input::IsKeyTrigger(DIK_F5)) {
		mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
		block_->SetMapChipFieldBlocks(mapChipField_.get());
	}

	// Rキーでプレイヤーの位置をリセット
	if (Input::IsKeyTrigger(DIK_R)) {
		player_->SetPosition(mapChipField_->GetMapChipPosition(1, kNumBlockVertical - 2));
	}

#endif // _DEBUG
}

void GameScene::Draw() {
    sRenderer->PreDraw();

	// 自キャラの描画
	player_->Draw();
	// ブロックの描画
	block_->Draw();
	// スカイドームの描画
	skydome_->Draw();

    sRenderer->PostDraw();
}
