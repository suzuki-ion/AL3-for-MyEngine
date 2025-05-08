#include <cassert>
#include <format>
#include <chrono>
#include <2d/ImGuiManager.h>

#include "WinApp.h"
#include "Common/Logs.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace MyEngine {

namespace {

// ref: https://devblogs.microsoft.com/oldnewthing/20131017-00/?p=2903
BOOL UnadjustWindowRectEx(LPRECT prc, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle) {
    RECT rc;
    SetRectEmpty(&rc);
    BOOL fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
    if (fRc) {
        prc->left -= rc.left;
        prc->top -= rc.top;
        prc->right -= rc.right;
        prc->bottom -= rc.bottom;
    }
    return fRc;
}

} // namespace

WinApp::WinApp(const std::wstring &title, UINT windowStyle, int32_t width, int32_t height) {
    //==================================================
    // ウィンドウの初期化
    //==================================================
    
    // ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc_.lpszClassName = L"Window";
    // インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // ウィンドウクラスを登録
    RegisterClass(&wc_);

    // クライアントサイズの横幅
    clientWidth_ = width;
    // クライアントサイズの縦幅
    clientHeight_ = height;
    // ウィンドウスタイルを保存
    windowStyle_ = windowStyle;
    // アスペクト比を計算
    aspectRatio_ = static_cast<float>(clientWidth_) / static_cast<float>(clientHeight_);

    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    wrc_ = { 0, 0, clientWidth_, clientHeight_ };

    // クライアント領域を元に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc_, windowStyle_, false);

    // ウィンドウハンドルを作成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,      // 利用するクラス名
        title.c_str(),          // タイトルバーの文字
        windowStyle_,           // ウィンドウスタイル
        CW_USEDEFAULT,          // 表示X座標
        CW_USEDEFAULT,          // 表示Y座標
        wrc_.right - wrc_.left, // ウィンドウ横幅
        wrc_.bottom - wrc_.top, // ウィンドウ縦幅
        nullptr,                // 親ウィンドウハンドル
        nullptr,                // メニューハンドル
        wc_.hInstance,          // インスタンスハンドル
        nullptr                 // オプション
    );
    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // ウィンドウを表示
    ShowWindow(hwnd_, SW_NORMAL);

    // 初期化完了のログを出力
    Log("WinApp Initialized.");
    LogNewLine();
}

WinApp::~WinApp() {
    // ウィンドウを破棄
    DestroyWindow(hwnd_);
    // ウィンドウクラスを登録解除
    UnregisterClass(wc_.lpszClassName, wc_.hInstance);

    // 終了処理完了のログを出力
    Log("WinApp Finalized.");
    LogNewLine();
}

int32_t WinApp::GetClientWidth() const {
    RECT wrc = {};
    GetClientRect(hwnd_, &wrc);
    return wrc.right - wrc.left;
}

int32_t WinApp::GetClientHeight() const {
    RECT wrc = {};
    GetClientRect(hwnd_, &wrc);
    return wrc.bottom - wrc.top;
}

void WinApp::SetSizeChangeMode(SizeChangeMode mode) {
    // サイズ変更モードを保存
    sizeChangeMode_ = mode;
    if (mode == SizeChangeMode::kNone) {
        windowStyle_ &= ~WS_THICKFRAME;
    } else {
        if (mode == SizeChangeMode::kFixedAspect) {
            // 現在のアスペクト比を保存
            RECT wrc = {};
            GetClientRect(hwnd_, &wrc);
            aspectRatio_ = static_cast<float>(wrc.right - wrc.left) / static_cast<float>(wrc.bottom - wrc.top);
        }
        windowStyle_ |= WS_THICKFRAME;
    }

    // ウィンドウのサイズを変更
    SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
    SetWindowPos(
        hwnd_,
        nullptr,
        0,
        0,
        0,
        0,
        (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED)
    );
    ShowWindow(hwnd_, SW_NORMAL);
}

int WinApp::ProccessMessage() {
    if (msg_.message == WM_QUIT) {
        LogNewLine();
        Log("Window Quit.");
        return -1;
    }

    if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg_);
        DispatchMessage(&msg_);

    } else {
        return 0;
    }
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    WinApp *winApp = reinterpret_cast<WinApp *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return true;
    }
    
    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された時
        case WM_DESTROY:
            // OSに対して。アプリの終了を伝える
            PostQuitMessage(0);
            return 0;

        case WM_SIZING:
            if (winApp == nullptr) {
                break;
            }

            if (winApp->GetSizeChangeMode() == SizeChangeMode::kFixedAspect) {
                float aspectRatio = winApp->aspectRatio_;
                float aspectRatioRecp = 1.0f / aspectRatio;
                RECT *wrc = reinterpret_cast<RECT *>(&lparam);
                UnadjustWindowRectEx(wrc, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != 0,
                    GetWindowLong(hwnd, GWL_EXSTYLE));

                switch (wparam) {
                    case WMSZ_LEFT:
                    case WMSZ_BOTTOMLEFT:
                    case WMSZ_RIGHT:
                    case WMSZ_BOTTOMRIGHT:
                        wrc->bottom = wrc->top + LONG((wrc->right - wrc->left) * aspectRatioRecp);
                        break;

                    case WMSZ_TOP:
                    case WMSZ_TOPRIGHT:
                    case WMSZ_BOTTOM:
                        wrc->right = wrc->left + LONG((wrc->bottom - wrc->top) * aspectRatio);
                        break;

                    case WMSZ_TOPLEFT:
                        wrc->top = wrc->bottom - LONG((wrc->right - wrc->left) * aspectRatioRecp);
                        wrc->left = wrc->right - LONG((wrc->bottom - wrc->top) * aspectRatio);
                        break;
                }
                AdjustWindowRectEx(wrc, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != 0,
                    GetWindowLong(hwnd, GWL_EXSTYLE));
            }
            break;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

} // namespace MyEngine