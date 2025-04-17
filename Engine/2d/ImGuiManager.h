#pragma once
#include <d3d12.h>
#include <imgui.h>
#include <wrl.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace MyEngine {

class ImGuiManager final {
public:
    ImGuiManager(const ImGuiManager &) = delete;
    ImGuiManager &operator=(const ImGuiManager &) = delete;
    ImGuiManager(ImGuiManager &&) = delete;
    ImGuiManager &operator=(ImGuiManager &&) = delete;

    /// @brief インスタンス取得
    /// @return ImGuiManagerクラスのインスタンス
    static ImGuiManager *GetInstance() {
        static ImGuiManager instance;
        return &instance;
    }

    /// @brief ImGui初期化
    void Initialize();

    /// @brief ImGui終了処理
    void Finalize();

    /// @brief ImGuiのフレーム開始処理
    void BeginFrame();

    /// @brief ImGuiのフレーム終了処理
    void EndFrame();

private:
    ImGuiManager() = default;
    ~ImGuiManager() = default;

    /// @brief ImGui用のSRV用ディスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
};

} // namespace MyEngine