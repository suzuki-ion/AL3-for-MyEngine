#include "Engine/Engine.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    Engine *engine = Engine::GetInstance();
    engine->Initialize("MyEngine", 1280, 720, true);

    // ウィンドウのxボタンが押されるまでループ
    while (engine->ProccessMessage() != -1) {
        engine->BeginFrame();
        
        // ゲームの処理

        engine->EndFrame();
    }

    engine->Finalize();
    return 0;
}