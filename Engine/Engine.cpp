#include <cassert>
#include <vector>

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Base/ConvertString.h"
#include "Base/CrashHandler.h"
#include "Base/VertexData.h"
#include "Base/ConvertColor.h"
#include "2d/ImGuiManager.h"
#include "3d/PrimitiveDrawer.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/RenderingPipeline.h"
#include "Engine.h"

using namespace MyEngine;

namespace {
    // 各エンジン用クラスのグローバル変数
    WinApp *sWinApp = nullptr;
    DirectXCommon *sDxCommon = nullptr;
    PrimitiveDrawer *sPrimitiveDrawer = nullptr;
    TextureManager *sTextureManager = nullptr;
    ImGuiManager *sImGuiManager = nullptr;
} // namespace

void Engine::Initialize(const char *title, int width, int height, bool enableDebugLayer) {
    // エンジン初期化済みならエラー
    assert(!sWinApp);
    assert(!sDxCommon);
    assert(!sPrimitiveDrawer);
    assert(!sTextureManager);
    assert(!sImGuiManager);

    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // 誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
    SetUnhandledExceptionFilter(ExportDump);

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

    // ImGui初期化
    sImGuiManager = ImGuiManager::GetInstance();
    sImGuiManager->Initialize();

    // テクスチャ管理クラス初期化
    sTextureManager = TextureManager::GetInstance();
    sTextureManager->Initialize();
    // テスト用にテクスチャを読み込む
    sTextureManager->Load("Resources/uvChecker.png");

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Engine.");
}

void Engine::Finalize() {
    sTextureManager->Finalize();
    sImGuiManager->Finalize();
    sPrimitiveDrawer->Finalize();
    sDxCommon->Finalize();
    sWinApp->Finalize();
    sTextureManager = nullptr;
    sImGuiManager = nullptr;
    sPrimitiveDrawer = nullptr;
    sDxCommon = nullptr;
    sWinApp = nullptr;
    CoUninitialize();
}

void Engine::BeginFrame() {
    sDxCommon->PreDraw();
    sImGuiManager->BeginFrame();

    ID3D12DescriptorHeap *descriptorHeaps[] = { sImGuiManager->GetSRVDescriptorHeap() };
    sDxCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void Engine::EndFrame() {
    sImGuiManager->EndFrame();
    sDxCommon->PostDraw();
}

void Engine::DrawTest() {
    static auto *commandList = sDxCommon->GetCommandList();
    static auto mesh = sPrimitiveDrawer->CreateMesh(6);
    static auto pipelineSet = sPrimitiveDrawer->CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    // マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
    static auto materialResource = sPrimitiveDrawer->CreateBufferResources(sizeof(Vector4));
    static Vector4 color = { 255.0f, 255.0f, 255.0f, 255.0f };
    Vector4 *materialData = nullptr;
    materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData));

    // WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    static auto wvpResource = sPrimitiveDrawer->CreateBufferResources(sizeof(Matrix4x4));
    Matrix4x4 *wvpData = nullptr;
    wvpResource->Map(0, nullptr, reinterpret_cast<void **>(&wvpData));
    
    // 回転用にTransform変数を作る
    static Transform transform{
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
    transform.rotate.y += 0.005f;
    static Transform cameraTransform{
        {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -5.0f}
    };
    Matrix4x4 worldMatrix;
    Matrix4x4 cameraMatrix;
    Matrix4x4 viewMatrix;
    Matrix4x4 projectionMatrix;
    Matrix4x4 wvpMatrix;

    // ImGuiで三角形をいじれるようにする
    ImGui::Begin("Triangle");
    ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);
    ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
    ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
    ImGui::DragFloat4("MaterialColor", &color.x, 1.0f, 0.0f, 255.0f);
    ImGui::End();

    // 色をマテリアルに反映
    *materialData = ConvertColor(color);

    worldMatrix.MakeAffine(
        transform.scale,
        transform.rotate,
        transform.translate
    );
    cameraMatrix.MakeAffine(
        cameraTransform.scale,
        cameraTransform.rotate,
        cameraTransform.translate
    );
    viewMatrix = cameraMatrix.Inverse();
    projectionMatrix = MakePerspectiveFovMatrix(
        0.45f,
        static_cast<float>(sWinApp->GetClientWidth()) /
        static_cast<float>(sWinApp->GetClientHeight()),
        0.1f, 100.0f
    );
    wvpMatrix = worldMatrix * (viewMatrix * projectionMatrix);

    *wvpData = wvpMatrix;

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
    // マテリアルCBufferの場所を指定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
    // wvp用のCBufferの場所を指定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    commandList->SetGraphicsRootDescriptorTable(2, sTextureManager->GetTextureSrvHandleGPU());
    // 描画
    commandList->DrawInstanced(6, 1, 0, 0);
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