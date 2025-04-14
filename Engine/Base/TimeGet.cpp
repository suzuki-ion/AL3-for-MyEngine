#include "TimeGet.h"
#include <format>

namespace MyEngine {

std::chrono::zoned_seconds TimeGetZone() {
    // 現在時刻を取得（UTC時刻）
    auto now = std::chrono::system_clock::now();
    // ログファイル用なのでコンマ秒は削る
    auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    // 日本時間（PCの設定時間）に変換して返す
    return std::chrono::zoned_time{ std::chrono::current_zone(), nowSeconds };
}

std::string TimeGetString() {
    // フォーマットを使って年月日_時分秒の文字列に変換
    auto time = TimeGetZone();
    auto timeString = std::format("%Y/%m/%d %H:%M:%S", time);
    return timeString;
}

std::wstring TimeGetStringW() {
    // フォーマットを使って年月日_時分秒の文字列に変換
    auto time = TimeGetZone();
    auto timeString = std::format(L"%Y/%m/%d %H:%M:%S", time);
    return timeString;
}

} // namespace MyEngine