#include <cassert>
#include "Engine.h"
#include "Base/ConvertString.h"
#include "Base/DirectXCommon.h"
#include "Base/WinApp.h"

namespace {
    // 各エンジン用クラスのグローバル変数
    WinApp *sWinApp = nullptr;
    DirectXCommon *sDxCommon = nullptr;
}

void Engine::Initialize(const char *title, int width, int height, bool enableDebugLayer) {
    // エンジン初期化済みならエラー
    assert(!sWinApp);
    assert(!sDxCommon);

    // タイトル名がそのままだと使えないので変換
    std::wstring wTitle = ConvertString(title);
    // Windowsアプリ初期化
    sWinApp = WinApp::GetInstance();
    sWinApp->Initialize(wTitle, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
        width, height);

    // DirectX初期化
    sDxCommon = DirectXCommon::GetInstance();
    sDxCommon->Initialize(enableDebugLayer);
}

void Engine::Finalize() {
    sDxCommon->Finalize();
    sWinApp->Finalize();
    sDxCommon = nullptr;
    sWinApp = nullptr;
}

void Engine::BeginFrame() {
    sDxCommon->PreDraw();
}

void Engine::EndFrame() {
    sDxCommon->PostDraw();
}

HWND Engine::GetWindowHandle() const {
    return sWinApp->GetWindowHandle();
}

int32_t Engine::GetClientWidth() const {
    return sWinApp->GetClientWidth();
}

int32_t Engine::GetClientHeight() const {
    return sWinApp->GetClientHeight();
}

int Engine::ProccessMessage() {
    return sWinApp->ProccessMessage();
}

Engine::FinalizeChecker::~FinalizeChecker() {
    // エンジンが完全に終了しているかチェック
    assert(sWinApp == nullptr);
    assert(sDxCommon == nullptr);
}