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

/// @brief ログ出力用のテキストを作成
/// @param message ログメッセージ
/// @param isError エラーログかどうか
/// @param location ソースロケーション
/// @return ログ出力用のテキスト
std::string CreateLogText(const std::string &message, bool isError, const std::source_location &location) {
    std::string logText;
    logText += TimeGetString("[ {:%Y/%m/%d %H:%M:%S} - ");
    logText += "File:\"";
    logText += location.file_name();
    logText += "\" Function:\"";
    logText += location.function_name();
    logText += "\" Line:";
    logText += location.line();
    logText += "] : ";
    if (isError) {
        logText += "[ERROR]";
    }
    logText += message;
    logText += '\0';
    return logText;
}

} // namespace

void InitializeLog(const std::string &filePath) {
    // ログファイルのパスを作成
    std::filesystem::create_directory(filePath);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = filePath + '/' + TimeGetString("{:%Y-%m-%d_%H-%M-%S}") + ".log";
    // ファイルを使って書き込み準備
    logStream.open(logFilePath);
}

void Log(const std::string &message, bool isError, const std::source_location &location) {
    // ログテキストを作成
    std::string logText = CreateLogText(message, isError, location);
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA(logText.c_str());
}

void Log(const std::wstring &message, bool isError, const std::source_location &location) {
    // ログテキストを作成
    std::string logText = CreateLogText(ConvertString(message), isError, location);
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA(logText.c_str());
}

} // namespace MyEngine