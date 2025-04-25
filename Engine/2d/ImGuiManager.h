#pragma once
#include <d3d12.h>
#include <imgui.h>
#include <wrl.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace MyEngine {

// 前方宣言
class WinApp;
class DirectXCommon;

class ImGuiManager final {
public:
    ImGuiManager(WinApp *winApp, DirectXCommon *dxCommon);
    ~ImGuiManager();

    /// @brief ImGuiのフレーム開始処理
    void BeginFrame();

    /// @brief ImGuiのフレーム終了処理
    void EndFrame();

    /// @brief SRVディスクリプタヒープ取得
    /// @return SRVディスクリプタヒープ
    ID3D12DescriptorHeap *GetSRVDescriptorHeap() const {
        return srvDescriptorHeap_.Get();
    }

private:
    /// @brief WinAppインスタンス
    WinApp *winApp_ = nullptr;
    /// @brief DirectXCommonインスタンス
    DirectXCommon *dxCommon_ = nullptr;

    /// @brief SRVディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
};

} // namespace MyEngine