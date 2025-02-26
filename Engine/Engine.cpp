#include "Engine.h"

#include "Base/WinApp.h"
#include "Base/DirectXCommon.h"

void Engine::Initialize() {
    winApp_ = WinApp::GetInstance();
    dxCommon_ = DirectXCommon::GetInstance();

    winApp_->Initialize();
    dxCommon_->Initialize();
}

void Engine::Finalize() {

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