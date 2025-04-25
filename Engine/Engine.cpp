#include <cassert>
#include <vector>
#include <memory>

#include "Common/ConvertString.h"
#include "Common/VertexData.h"
#include "Common/ConvertColor.h"
#include "Common/Logs.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Base/CrashHandler.h"
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

// 各エンジン用クラスのグローバル変数
std::unique_ptr<WinApp> sWinApp;
std::unique_ptr<DirectXCommon> sDxCommon;
std::unique_ptr<PrimitiveDrawer> sPrimitiveDrawer;
std::unique_ptr<TextureManager> sTextureManager;
std::unique_ptr<ImGuiManager> sImGuiManager;
} // namespace

void Engine::Initialize(const char *title, int width, int height, bool enableDebugLayer) {
    // ログの初期化
    InitializeLog("Logs");

    // 初期化済みかどうかのフラグ
    static bool isInitialized = false;
    // 初期化済みならエラーを出す
    if (isInitialized) {
        Log("Engine is already initialized.", true);
        return;
    }
    // 初期化済みフラグを立てる
    isInitialized = true;

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

    // DirectX初期化
    sDxCommon = std::make_unique<DirectXCommon>(enableDebugLayer, sWinApp.get());

    // プリミティブ描画クラス初期化
    sPrimitiveDrawer = std::make_unique<PrimitiveDrawer>(sWinApp.get(), sDxCommon.get());

    // ImGui初期化
    sImGuiManager = std::make_unique<ImGuiManager>(sWinApp.get(), sDxCommon.get());

    // テクスチャ管理クラス初期化
    sTextureManager = std::make_unique<TextureManager>(sWinApp.get(), sDxCommon.get(), sPrimitiveDrawer.get(), sImGuiManager.get());
    // テクスチャを読み込む
    sTextureManager->Load("Resources/uvChecker.png");

    // 初期化完了のログを出力
    Log("Initialized.");
}

void Engine::Finalize() {
    sTextureManager.reset();
    sImGuiManager.reset();
    sPrimitiveDrawer.reset();
    sDxCommon.reset();
    sWinApp.reset();
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

FinalizeChecker::~FinalizeChecker() {
    // エンジンが完全に終了しているかチェック
    assert(!sWinApp);
    assert(!sDxCommon);
    assert(!sPrimitiveDrawer);
    assert(!sTextureManager);
    assert(!sImGuiManager);
    // 初期化完了のログを出力
    Log("Complete Finalize Engine.");
}