#include <Base/Input.h>
#include <dinput.h>
#include <cassert>
#include <unordered_map>
#include <array>

#include "KeyConfig.h"

using namespace KashipanEngine;

namespace {

// キーコンフィグの設定
//int keyConfig[static_cast<int>(Action::kActions)][static_cast<int>(Action::kKeyConfigNum)] = {
//    { DIK_W, DIK_UP },
//    { DIK_S, DIK_DOWN },
//    { DIK_A, DIK_LEFT },
//    { DIK_D, DIK_RIGHT },
//    { DIK_SPACE, DIK_RETURN }
//};
std::unordered_map<Action, std::array<int, static_cast<int>(Action::kKeyConfigNum)>> keyConfig = {
    // 移動
    { Action::kMoveUp, { DIK_W, -1 } },
    { Action::kMoveDown, { DIK_S, -1 } },
    { Action::kMoveLeft, { DIK_A, -1 } },
    { Action::kMoveRight, { DIK_D, -1 } },
    // 回転
    { Action::kRotateUp, { DIK_UP, -1 } },
    { Action::kRotateDown, { DIK_DOWN, -1 } },
    { Action::kRotateLeft, { DIK_LEFT, -1 } },
    { Action::kRotateRight, { DIK_RIGHT, -1 } },
    // 弾の発射
    { Action::kShootBullet, { DIK_SPACE, DIK_RETURN } }
};

} // namespace

bool IsAction(Action actionType) {
    for (auto key : keyConfig[actionType]) {
        // 指定の操作のキーが押されているかを確認
        if (Input::IsKeyDown(key)) {
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
    return keyConfig[actionType][index];
}

void ConfigSet(Action actionType, int index, int keyID) {
    // 指定の操作が範囲外の場合はエラー
    if (static_cast<int>(actionType) < 0 || static_cast<int>(actionType) >= static_cast<int>(Action::kActions) ||
        index < 0 || index >= static_cast<int>(Action::kKeyConfigNum)) {
        assert(false);
    }
    keyConfig[actionType][index] = keyID;
}

