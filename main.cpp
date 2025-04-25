#include <Engine.h>
#include <memory>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // エンジンのインスタンスを作成
    std::unique_ptr<Engine> engine = std::make_unique<Engine>();
    // エンジンの初期化
    engine->Initialize("MyEngine", 1280, 720, true);

    // ウィンドウのxボタンが押されるまでループ
    while (engine->ProccessMessage() != -1) {
        engine->BeginFrame();
        
        engine->DrawTest();

        engine->EndFrame();
    }

    engine->Finalize();
    return 0;
}