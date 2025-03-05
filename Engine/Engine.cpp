#include <cassert>

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/ConvertString.h"
#include "3d/PrimitiveDrawer.h"
#include "Engine.h"

using namespace MyEngine;

namespace {
    // 各エンジン用クラスのグローバル変数
    WinApp *sWinApp = nullptr;
    DirectXCommon *sDxCommon = nullptr;
    PrimitiveDrawer *sPrimitiveDrawer = nullptr;
}

void Engine::Initialize(const char *title, int width, int height, bool enableDebugLayer) {
    // エンジン初期化済みならエラー
    assert(!sWinApp);
    assert(!sDxCommon);
    assert(!sPrimitiveDrawer);

    // タイトル名がそのままだと使えないので変換
    std::wstring wTitle = ConvertString(title);
    // Windowsアプリ初期化
    sWinApp = WinApp::GetInstance();
    sWinApp->Initialize(wTitle, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
        width, height);

    // DirectX初期化
    sDxCommon = DirectXCommon::GetInstance();
    sDxCommon->Initialize(enableDebugLayer);

    // プリミティブ描画クラス初期化
    sPrimitiveDrawer = PrimitiveDrawer::GetInstance();
    sPrimitiveDrawer->Initialize();

    // テスト描画用

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Engine.\n");
}

void Engine::Finalize() {
    sPrimitiveDrawer->Finalize();
    sDxCommon->Finalize();
    sWinApp->Finalize();
    sPrimitiveDrawer = nullptr;
    sDxCommon = nullptr;
    sWinApp = nullptr;
}

void Engine::BeginFrame() {
    sDxCommon->PreDraw();
}

void Engine::EndFrame() {
    sDxCommon->PostDraw();
}

void Engine::DrawTest() {
    static auto *commandList = sDxCommon->GetCommandList();
    static auto mesh = sPrimitiveDrawer->CreateMesh(3);
    static auto pipelineSet = sPrimitiveDrawer->CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    // ビューポート
    D3D12_VIEWPORT viewport{};
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(sWinApp->GetClientWidth());
    viewport.Height = static_cast<float>(sWinApp->GetClientHeight());
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // シザー矩形
    D3D12_RECT scissorRect{};
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = sWinApp->GetClientWidth();
    scissorRect.top = 0;
    scissorRect.bottom = sWinApp->GetClientHeight();

    // コマンドを積む
    commandList->RSSetViewports(1, &viewport);          // ビューポートを設定
    commandList->RSSetScissorRects(1, &scissorRect);    // シザー矩形を設定
    // ルートシグネチャを設定。PSOに設定しているけど別途設定が必要
    commandList->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());
    commandList->SetPipelineState(pipelineSet->pipelineState.Get());    // PSOを設定
    commandList->IASetVertexBuffers(0, 1, &mesh->vertexBufferView);     // VBVを設定
    // 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // 描画
    commandList->DrawInstanced(3, 1, 0, 0);
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
    assert(sPrimitiveDrawer == nullptr);
}