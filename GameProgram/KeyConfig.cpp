#include <Base/Input.h>
#include <dinput.h>
#include <cassert>
#include <unordered_map>
#include <array>

#include "KeyConfig.h"

using namespace KashipanEngine;

namespace {

// キー設定用の構造体
struct KeyConfig {
    // 設定されてるキー
    std::array<int, static_cast<int>(Action::kKeyConfigNum)> keys;
    // 判定用関数へのポインタ
    bool (*isKeyPressed)(int keyID);
};

// キーコンフィグの設定
std::unordered_map<Action, KeyConfig> keyConfig = {
    // 移動
    { Action::kMoveUp, {{ DIK_W, -1 }, Input::IsKeyDown }},
    { Action::kMoveDown, {{ DIK_S, -1 }, Input::IsKeyDown }},
    { Action::kMoveLeft, {{ DIK_A, -1 }, Input::IsKeyDown }},
    { Action::kMoveRight, {{ DIK_D, -1 }, Input::IsKeyDown }},
    // 回転
    { Action::kRotateUp, {{ DIK_UP, -1 }, Input::IsKeyDown }},
    { Action::kRotateDown, {{ DIK_DOWN, -1 }, Input::IsKeyDown }},
    { Action::kRotateLeft, {{ DIK_LEFT, -1 }, Input::IsKeyDown }},
    { Action::kRotateRight, {{ DIK_RIGHT, -1 }, Input::IsKeyDown }},
    // 弾の発射
    { Action::kShootBullet, {{ DIK_SPACE, DIK_RETURN }, Input::IsKeyTrigger }}
};

} // namespace

bool IsAction(Action actionType) {
    for (auto key : keyConfig[actionType].keys) {
        // キーIDが-1の場合は設定されていないキーなのでスキップ
        if (key == -1) {
            continue;
        }
        // 指定の操作のキーが押されているかを確認
        if (keyConfig[actionType].isKeyPressed(key)) {
            return true;
        }
    }
    return false;
}

int ConfigGet(Action actionType, int index) {
    // 指定の操作が範囲外の場合はエラー
    if (static_cast<int>(actionType) < 0 || static_cast<int>(actionType) >= static_cast<int>(Action::kActions) ||
        index < 0 || index >= static_cast<int>(Action::kKeyConfigNum)) {
        assert(false);
    }
    return keyConfig[actionType].keys[index];
}

void ConfigSet(Action actionType, int index, int keyID) {
    // 指定の操作が範囲外の場合はエラー
    if (static_cast<int>(actionType) < 0 || static_cast<int>(actionType) >= static_cast<int>(Action::kActions) ||
        index < 0 || index >= static_cast<int>(Action::kKeyConfigNum)) {
        assert(false);
    }
    keyConfig[actionType].keys[index] = keyID;
}

