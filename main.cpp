#include "Engine/Engine.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine *engine = Engine::GetInstance();
    engine->Initialize();

    // ウィンドウのxボタンが押されるまでループ
    while (engine->ProccessMessage() != -1) {
        // ゲームの処理
    }

    engine->Finalize();
    return 0;
}