#include "Engine.h"
#include "WinApp.h"

void Engine::Initialize() {
    winApp_ = new WinApp();
    winApp_->Initialize();
}

void Engine::Finalize() {
    delete winApp_;
}

HWND Engine::GetWindowHandle() const {
    return winApp_->GetWindowHandle();
}

int32_t Engine::GetClientWidth() const {
    return winApp_->GetClientWidth();
}

int32_t Engine::GetClientHeight() const {
    return winApp_->GetClientHeight();
}

int Engine::ProccessMessage() {
    return winApp_->ProccessMessage();
}