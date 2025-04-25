#pragma once
#include <string>
#include <source_location>

namespace MyEngine {

/// @brief ログ初期化
/// @param filePath ログファイルを保存するフォルダへのパス
/// @param projectDir プロジェクトのルートディレクトリへのパス
void InitializeLog(const std::string &filePath, const std::string &projectDir);

/// @brief ログ出力
/// @param message ログメッセージ
/// @param isError エラーログかどうか
/// @param location ソースロケーション
void Log(const std::string &message, bool isError = false, const std::source_location &location = std::source_location::current());

/// @brief ログ出力
/// @param message ログメッセージ
/// @param isError エラーログかどうか
/// @param location ソースロケーション
void Log(const std::wstring &message, bool isError = false, const std::source_location &location = std::source_location::current());

} // namespace MyEngine