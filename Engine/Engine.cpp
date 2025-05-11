#include <cassert>
#include <vector>
#include <memory>

#include "Common/ConvertString.h"
#include "Common/VertexData.h"
#include "Common/ConvertColor.h"
#include "Common/Logs.h"
#include "Common/Descriptors/RTV.h"
#include "Common/Descriptors/SRV.h"
#include "Common/Descriptors/DSV.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Base/CrashHandler.h"
#include "Base/ResourceLeakChecker.h"
#include "Base/Drawer.h"
#include "Base/Input.h"
#include "2d/ImGuiManager.h"
#include "3d/PrimitiveDrawer.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/RenderingPipeline.h"
#include "Engine.h"

using namespace MyEngine;

namespace {
/// @brief 終了処理チェック用構造体
struct FinalizeChecker {
    ~FinalizeChecker();
};
/// @brief 終了処理チェック用変数
FinalizeChecker finalizeCheck_;
/// @brief リソースリークチェック用変数
D3DResourceLeakChecker leakCheck_;

// 各エンジン用クラスのグローバル変数
std::unique_ptr<WinApp> sWinApp;
std::unique_ptr<DirectXCommon> sDxCommon;
std::unique_ptr<TextureManager> sTextureManager;
std::unique_ptr<ImGuiManager> sImGuiManager;
std::unique_ptr<Drawer> sDrawer;
} // namespace

Engine::Engine(const char *title, int width, int height, bool enableDebugLayer,
    const std::filesystem::path &projectDir) {
    // ログの初期化
    InitializeLog("Logs", projectDir.string());
    LogInsertPartition("\n================ Engine Initialize ===============\n");

    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // 誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

    // タイトル名がそのままだと使えないので変換
    std::wstring wTitle = ConvertString(title);
    // Windowsアプリ初期化
    sWinApp = std::make_unique<WinApp>(
        wTitle,
        WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
        width,
        height
    );
    sWinApp->SetSizeChangeMode(MyEngine::SizeChangeMode::kNone);

    // DirectX初期化
    sDxCommon = std::make_unique<DirectXCommon>(enableDebugLayer, sWinApp.get());

    // InputManager初期化
    Input::Initialize(sWinApp.get());

    // プリミティブ描画クラス初期化
    PrimitiveDrawer::Initialize(sDxCommon.get());

    // ImGui初期化
    sImGuiManager = std::make_unique<ImGuiManager>(sWinApp.get(), sDxCommon.get());

    // テクスチャ管理クラス初期化
    sTextureManager = std::make_unique<TextureManager>(sWinApp.get(), sDxCommon.get());

    // 描画用クラス初期化
    sDrawer = std::make_unique<Drawer>(sWinApp.get(), sDxCommon.get(), sImGuiManager.get(), sTextureManager.get());

    // 初期化完了のログを出力
    Log("Engine Initialized.");
    LogInsertPartition("\n============ Engine Initialize Finish ============\n");
}

Engine::~Engine() {
    LogInsertPartition("\n================= Engine Finalize ================\n");
    sTextureManager.reset();
    sImGuiManager.reset();
    sDxCommon.reset();
    sWinApp.reset();
    // DescriptorHeapの解放
    RTV::Finalize();
    DSV::Finalize();
    SRV::Finalize();
    CoUninitialize();
    // 終了処理完了のログを出力
    Log("Engine Finalized.");
    LogInsertPartition("\n============= Engine Finalize Finish =============\n");
}

void Engine::BeginFrame() {
    sDrawer->PreDraw();
}

void Engine::EndFrame() {
    sDrawer->PostDraw();
}

MyEngine::WinApp *Engine::GetWinApp() const {
    return sWinApp.get();
}

MyEngine::DirectXCommon *Engine::GetDxCommon() const {
    return sDxCommon.get();
}

MyEngine::Drawer *Engine::GetDrawer() const {
    return sDrawer.get();
}

MyEngine::TextureManager *Engine::GetTextureManager() const {
    return sTextureManager.get();
}

int Engine::ProccessMessage() {
    return sWinApp->ProccessMessage();
}

FinalizeChecker::~FinalizeChecker() {
    // エンジンが完全に終了しているかチェック
    assert(!sWinApp);
    assert(!sDxCommon);
    assert(!sTextureManager);
    assert(!sImGuiManager);
    // 初期化完了のログを出力
    Log("Complete Finalize Engine.");
}