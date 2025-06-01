#include <GameScene.h>
#include <memory>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
    gameScene->GameLoop();
    return 0;
}