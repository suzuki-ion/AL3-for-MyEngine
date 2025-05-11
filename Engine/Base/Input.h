#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace MyEngine{

// 前方宣言
class WinApp;

// 入力はどこでも使えるようにしたいのでstaticにする

/// @brief 入力管理クラス
class Input {
public:
    Input() = delete;
    Input(const Input &) = delete;
    Input(Input &&) = delete;
    Input &operator=(const Input &) = delete;

    /// @brief 初期化処理
    /// @param winApp WinAppインスタンス
    static void Initialize(WinApp *winApp);

    /// @brief 終了処理
    static void Finalize();

    /// @brief 入力状態更新
    static void Update();

    /// @brief キーの押下状態を取得
    /// @param key キーコード
    /// @return 押下状態
    static bool IsKeyDown(int key) {
        return (keyboardState_[key] & 0x80) != 0;
    }

    /// @brief 前回のキーの押下状態を取得
    /// @param key キーコード
    /// @return 前回の押下状態
    static bool IsPreKeyDown(int key) {
        return (preKeyboardState_[key] & 0x80) != 0;
    }

    /// @brief キーのトリガー状態を取得
    /// @param key キーコード
    /// @return トリガー状態
    static bool IsKeyTrigger(int key) {
        return (keyboardState_[key] & 0x80) != 0 && (preKeyboardState_[key] & 0x80) == 0;
    }

    /// @brief キーのリリース状態を取得
    /// @param key キーコード
    /// @return リリース状態
    static bool IsKeyRelease(int key) {
        return (keyboardState_[key] & 0x80) == 0 && (preKeyboardState_[key] & 0x80) != 0;
    }

private:
    /// @brief 初期化済みフラグ
    static bool isInitialized_;
    /// @brief DirectInputインターフェース
    static IDirectInput8 *directinput_;
    /// @brief キーボードデバイス
    static IDirectInputDevice8 *keyboardDevice_;
    /// @brief マウスデバイス
    static IDirectInputDevice8 *mouseDevice_;

    /// @brief キーボードの状態
    static BYTE keyboardState_[256];
    /// @brief 前回のキーボードの状態
    static BYTE preKeyboardState_[256];

    /// @brief マウスの状態
    static DIMOUSESTATE mouseState_;
    /// @brief 前回のマウスの状態
    static DIMOUSESTATE preMouseState_;
};

} // namespace MyEngine