#pragma once
#include <chrono>
#include <string>

namespace MyEngine {

/// @brief PCの設定時間取得
/// @return 現在のPCの設定時間
std::chrono::zoned_seconds TimeGetZone();

/// @brief PCの設定時間を文字列で取得
/// @param format 取得する時間のフォーマット
/// @return フォーマットに従ったPCの設定時間
std::string TimeGetString();

/// @brief PCの設定時間を文字列で取得
/// @param format 取得する時間のフォーマット
/// @return フォーマットに従ったPCの設定時間
std::wstring TimeGetStringW();

} // namespace MyEngine