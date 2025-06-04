#include <GameScene.h>
#include <memory>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    auto kashipanEngine = std::make_unique<Engine>("Kashipan Engine Game", 1280, 720, true);
    auto gameScene = std::make_unique<GameScene>(kashipanEngine.get());
    gameScene->GameLoop();
    return 0;
}
