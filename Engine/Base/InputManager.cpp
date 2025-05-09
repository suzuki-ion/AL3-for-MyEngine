#include <cassert>

#include "InputManager.h"
#include "Base/WinApp.h"
#include "Common/Logs.h"

namespace MyEngine {

bool InputManager::isInitialized_ = false;
IDirectInput8 *InputManager::directinput_ = nullptr;
IDirectInputDevice8 *InputManager::keyboardDevice_ = nullptr;
IDirectInputDevice8 *InputManager::mouseDevice_ = nullptr;
BYTE InputManager::keyboardState_[256] = {};
BYTE InputManager::preKeyboardState_[256] = {};
DIMOUSESTATE InputManager::mouseState_ = {};
DIMOUSESTATE InputManager::preMouseState_ = {};

void InputManager::Initialize(WinApp *winApp) {
    // 初期化済みフラグをチェック
    if (isInitialized_) {
        Log("InputManager is already initialized.", kLogLevelFlagError);
        assert(false);
    }

    // DirectInputの初期化
    HRESULT hr = DirectInput8Create(
        winApp->GetWindowClass().hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        reinterpret_cast<void **>(&directinput_),
        nullptr
    );
    assert(SUCCEEDED(hr));
    
    // キーボードデバイスの初期化
    hr = directinput_->CreateDevice(GUID_SysKeyboard, &keyboardDevice_, nullptr);
    assert(SUCCEEDED(hr));
    // マウスデバイスの初期化
    hr = directinput_->CreateDevice(GUID_SysMouse, &mouseDevice_, nullptr);
    assert(SUCCEEDED(hr));
    
    // デバイスの初期化
    hr = keyboardDevice_->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(hr));
    hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);
    assert(SUCCEEDED(hr));
    // 排他的レベルのセット
    hr = keyboardDevice_->SetCooperativeLevel(
        winApp->GetWindowHandle(),
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
    );
    assert(SUCCEEDED(hr));
    hr = mouseDevice_->SetCooperativeLevel(
        winApp->GetWindowHandle(),
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
    );
    assert(SUCCEEDED(hr));
    
    // 初期化完了のログを出力
    LogSimple("Complete Initialize InputManager.", kLogLevelFlagInfo);
    // 初期化済みフラグを立てる
    isInitialized_ = true;
}

void InputManager::Finalize() {
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("InputManager is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // 終了処理完了のログを出力
    Log("Complete Finalize InputManager.", kLogLevelFlagInfo);
}

void InputManager::Update() {
    // 初期化済みフラグをチェック
    if (!isInitialized_) {
        Log("InputManager is not initialized.", kLogLevelFlagError);
        assert(false);
    }

    // キーボードの状態を取得
    memcpy(preKeyboardState_, keyboardState_, sizeof(keyboardState_));
    keyboardDevice_->Acquire();
    keyboardDevice_->GetDeviceState(sizeof(keyboardState_), keyboardState_);

    // マウスの状態を取得
    memcpy(&preMouseState_, &mouseState_, sizeof(mouseState_));
    mouseDevice_->Acquire();
    mouseDevice_->GetDeviceState(sizeof(mouseState_), &mouseState_);
}

} // namespace MyEngine