#include "Engine.h"

LRESULT CALLBACK Engine::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された時
        case WM_DESTROY:
            // OSに対して。アプリの終了を伝える
            PostQuitMessage(0);
            return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Engine::Initialize() {
    // ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc_.lpszClassName = L"WindowClass";
    // インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // ウィンドウクラスを登録
    RegisterClass(&wc_);

    kClientWidth_ = 1280;
    kClientHeight_ = 720;

    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    wrc_ = { 0, 0, kClientWidth_, kClientHeight_ };

    // クライアント領域を元に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウハンドルを作成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,      // 利用するクラス名
        L"Window",              // タイトルバーの文字
        WS_OVERLAPPEDWINDOW,    // よく見るウィンドウスタイル
        CW_USEDEFAULT,          // 表示X座標
        CW_USEDEFAULT,          // 表示Y座標
        wrc_.right - wrc_.left, // ウィンドウ横幅
        wrc_.bottom - wrc_.top, // ウィンドウ縦幅
        nullptr,                // 親ウィンドウハンドル
        nullptr,                // メニューハンドル
        wc_.hInstance,          // インスタンスハンドル
        nullptr                 // オプション
    );

    // ウィンドウを表示
    ShowWindow(hwnd_, SW_SHOW);
}

int Engine::ProccessMessage() {
    if (msg_.message == WM_QUIT) {
        return -1;
    }

    if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg_);
        DispatchMessage(&msg_);

    } else {
        return 0;
    }
}
