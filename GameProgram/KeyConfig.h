#pragma once

//==================================================
// 操作の種類
//==================================================

enum class Action {
    // 移動
    kMoveUp,
    kMoveDown,
    kMoveLeft,
    kMoveRight,

    // 回転
    kRotateLeft,
    kRotateRight,
    kRotateUp,
    kRotateDown,

    // 弾の発射
    kShootBullet,

    // 総アクション数
    kActions,
    
    // 設定できるキーの数
    kKeyConfigNum = 2
};

//==================================================
// キーコンフィグ用関数
//==================================================

/// @brief 指定の操作がされているかを検知
/// @param actionType 操作の種類
/// @return true: 指定の操作がされている
bool IsAction(Action actionType);

/// @brief 指定の操作のキーを取得する
/// @param actionType 操作の種類
/// @param index 取得したいキーのインデックス
/// @return キーID
int ConfigGet(Action actionType, int index);

/// @brief 指定の操作のキーを設定する
/// @param actionType 操作の種類
/// @param index 設定したいキーのインデックス
/// @param keyID 設定するキー
void ConfigSet(Action actionType, int index, int keyID);

