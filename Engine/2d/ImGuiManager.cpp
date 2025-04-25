#include "ImGuiManager.h"
#include "Common/Logs.h"
#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"

#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

namespace MyEngine {

ImGuiManager::ImGuiManager(WinApp *winApp, DirectXCommon *dxCommon) {
    // nullチェック
    if (winApp == nullptr) {
        Log("winApp is null.", true);
        return;
    }
    if (dxCommon == nullptr) {
        Log("dxCommon is null.", true);
        return;
    }
    // 引数をメンバ変数に格納
    winApp_ = winApp;
    dxCommon_ = dxCommon;

    // srvDescriptorHeapの初期化
    srvDescriptorHeap_ = dxCommon_->CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true
    );

    // ImGuiの初期化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(winApp_->GetWindowHandle());
    ImGui_ImplDX12_Init(
        dxCommon_->GetDevice(),
        dxCommon_->GetSwapChainDesc().BufferCount,
        dxCommon_->GetRTVDesc().Format,
        srvDescriptorHeap_.Get(),
        srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
        srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
    );

    // 初期化完了のログを出力
    Log("ImGuiManager Initialized.");
}

ImGuiManager::~ImGuiManager() {
    // ImGuiの終了処理
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    // 終了処理完了のログを出力
    Log("ImGuiManager Finalized.");
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
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
}

} // namespace MyEngine