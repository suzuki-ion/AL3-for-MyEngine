#include <Windows.h>
#include <filesystem>
#include <fstream>
#include "Logs.h"
#include "TimeGet.h"
#include "ConvertString.h"

namespace MyEngine {

namespace {

// ログ出力用のストリーム
std::ofstream logStream;
// プロジェクトのルートディレクトリ
std::string projectDir;

/// @brief 相対パスを取得
/// @param fullPath フルパス
/// @return 相対パス
std::string GetRelativePath(const std::string &fullPath) {
    if (fullPath.find(projectDir) == 0) {
        return fullPath.substr(projectDir.length());
    }
    return fullPath;
}

/// @brief ログ出力用のテキストを作成
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
/// @param location ソースロケーション
/// @return ログ出力用のテキスト
std::string CreateLogText(const std::string &message, LogLevelFlags logLevelFlags, const std::source_location &location) {
    std::string logText;
    logText += TimeGetString("[ {:%Y/%m/%d %H:%M:%S} ]\n\t");
    logText += "[ ";
    logText += "File:\"";
    logText += GetRelativePath(location.file_name());
    logText += "\" Function:\"";
    logText += location.function_name();
    logText += "\" Line:";
    logText += std::to_string(location.line());
    logText += " ]\n\t";
    if (logLevelFlags & kLogLevelFlagInfo) {
        logText += "[INFO] : ";
    } else if (logLevelFlags & kLogLevelFlagWarning) {
        logText += "[--- WARNING ---] : ";
    } else if (logLevelFlags & kLogLevelFlagError) {
        logText += "[!!!--- ERROR ---!!!] : ";
    } else {
        logText += "[NO LEVEL LOG] : ";
    }
    logText += message;
    return logText;
}

} // namespace

void InitializeLog(const std::string &filePath, const std::string &projectDir) {
    // プロジェクトのルートディレクトリを保存
    MyEngine::projectDir = projectDir;

    // ログファイルのパスを作成
    std::filesystem::create_directory(filePath);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = filePath + '/' + TimeGetString("{:%Y-%m-%d_%H-%M-%S}") + ".log";
    // ファイルを使って書き込み準備
    logStream.open(logFilePath);

    // 初期化完了のログとプロジェクトのルートディレクトリをログに出力
    logStream << "Log initialized." << std::endl;
    logStream << "Project Directory: \"" << projectDir << "\"" << std::endl;
}

void Log(const std::string &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログテキストを作成
    std::string logText = CreateLogText(message, logLevelFlags, location);
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void Log(const std::wstring &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログテキストを作成
    std::string logText = CreateLogText(ConvertString(message), logLevelFlags, location);
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void LogNewLine() {
    logStream << std::endl;
    OutputDebugStringA("\n");
}

void LogInsertPartition(const std::string &partition) {
    logStream << partition << std::endl;
    OutputDebugStringA((partition + "\n").c_str());
}

} // namespace MyEngine