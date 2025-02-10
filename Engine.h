#pragma once
#include <Windows.h>
#include <cstdint>

class Engine final {
public:
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

    static Engine *GetInstance() {
        static Engine instance;
        return &instance;
    }

    void Initialize();
    
    HWND GetWindowHandle() const { return hwnd_; }
    int32_t GetClientWidth() const { return kClientWidth_; }
    int32_t GetClientHeight() const { return kClientHeight_; }
    int ProccessMessage();

private:
    Engine() = default;
    ~Engine() = default;

    WNDCLASS wc_{};
    HWND hwnd_{};
    int32_t kClientWidth_;
    int32_t kClientHeight_;
    RECT wrc_;
    MSG msg_;

    // ウィンドウプロシージャ
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};