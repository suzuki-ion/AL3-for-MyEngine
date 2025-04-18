#include "ImGuiManager.h"

#include <Base/WinApp.h>
#include <Base/DirectXCommon.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

namespace MyEngine {

namespace {
// 各エンジン用クラスのグローバル変数
WinApp *sWinApp = nullptr;
DirectXCommon *sDxCommon = nullptr;
} // namespace

void ImGuiManager::Initialize() {
    // WinAppのインスタンスを取得
    sWinApp = WinApp::GetInstance();
    // DirectXCommonのインスタンスを取得
    sDxCommon = DirectXCommon::GetInstance();

    // srvDescriptorHeapの初期化
    srvDescriptorHeap_ = sDxCommon->CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true
    );

    // ImGuiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(sWinApp->GetWindowHandle());
    ImGui_ImplDX12_Init(
        sDxCommon->GetDevice(),
        sDxCommon->GetSwapChainDesc().BufferCount,
        sDxCommon->GetRTVDesc().Format,
        srvDescriptorHeap_.Get(),
        srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
    );

    // ImGui初期化完了のログを出力
    sWinApp->Log("Complete Initialize ImGui.");
}

void ImGuiManager::Finalize() {
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // ImGui終了処理完了のログを出力
    sWinApp->Log("Complete Finalize ImGui.");
}

void ImGuiManager::BeginFrame() {
    // ImGuiのフレーム開始処理
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
    // ImGuiのフレーム終了処理
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), sDxCommon->GetCommandList());
}

} // namespace MyEngine