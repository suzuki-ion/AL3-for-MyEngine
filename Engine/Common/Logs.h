#pragma once
#include <string>
#include <source_location>

namespace MyEngine {

enum LogLevelFlags {
    kLogLevelFlagInfo = 0b0001,
    kLogLevelFlagWarning = 0b0010,
    kLogLevelFlagError = 0b0100
};

/// @brief ログ初期化
/// @param filePath ログファイルを保存するフォルダへのパス
/// @param projectDir プロジェクトのルートディレクトリへのパス
void InitializeLog(const std::string &filePath, const std::string &projectDir);

/// @brief ログ出力
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
/// @param location ソースロケーション
void Log(
    const std::string &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo,
    const std::source_location &location = std::source_location::current()
);

/// @brief ログ出力
/// @param message ログメッセージ
/// @param isError エラーログかどうか
/// @param location ソースロケーション
void Log(
    const std::wstring &message,
    const LogLevelFlags logLevelFlags = kLogLevelFlagInfo,
    const std::source_location &location = std::source_location::current()
);

/// @brief ログ改行
void LogNewLine();

/// @brief ログ仕切り
/// @param partition 仕切り文字列
void LogInsertPartition(const std::string &partition = "\n==================================================\n");

} // namespace MyEngine