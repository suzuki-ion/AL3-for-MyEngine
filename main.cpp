#include "GameScene.h"
#include "Base/Input.h"
#include "Base/Renderer.h"
#include "Base/Sound.h"

using namespace KashipanEngine;

namespace {

// エンジンのインスタンスを作成
auto sKashipanEngine = std::make_unique<Engine>("KashipanEngine", 1920, 1080, true);
// レンダラーへのポインタ
Renderer *sRenderer = sKashipanEngine->GetRenderer();

// ゲームシーンのインスタンスを生成
std::unique_ptr<GameScene> sGameScene = std::make_unique<GameScene>(sKashipanEngine.get());

} // namespace

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ウィンドウのxボタンが押されるまでループ
	while (sKashipanEngine->ProccessMessage() != -1) {
		sKashipanEngine->BeginFrame();
		if (sKashipanEngine->BeginGameLoop() == false) {
			continue;
		}
		Input::Update();

        // ゲームシーンの更新と描画
        sGameScene->Update();
        sGameScene->Draw();
	}

    sGameScene.reset();

	return 0;
}
