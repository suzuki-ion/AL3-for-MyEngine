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
// 出力するログの種類
LogLevelFlags outputLogLevel;
// ログレベルフラグの文字列
const char *logLevelFlagStrings[] = {
    "NONE",
    "INFO",
    "WARNING",
    "ERROR",
};

/// @brief 相対パスを取得
/// @param fullPath フルパス
/// @return 相対パス
std::string GetRelativePath(const std::string &fullPath) {
    if (fullPath.find(projectDir) == 0) {
        return fullPath.substr(projectDir.length());
    }
    return fullPath;
}

/// @brief ログ出力用の詳細情報テキストを作成
/// @param location ソースロケーション
/// @return 詳細情報テキスト
std::string CreateDetailLogText(const std::source_location &location) {
    std::string logText;
    logText += TimeGetString("[ {:%Y/%m/%d %H:%M:%S} ]\n\t");

    //--------- File ---------//
    logText += "File: \"";
    logText += GetRelativePath(location.file_name());
    logText += "\"\n\t";

    //--------- Function ---------//
    logText += "Function: \"";
    logText += location.function_name();
    logText += "\"\n\t";

    //--------- Line ---------//
    logText += "Line: ";
    logText += std::to_string(location.line());
    logText += "\n\t";

    return logText;
}

/// @brief ログ出力用の詳細情報無しのテキストを作成
/// @param message ログメッセージ
/// @param logLevelFlags ログレベルフラグ
/// @return ログ出力用のテキスト
std::string CreateLogText(const std::string &message, LogLevelFlags logLevelFlags) {
    std::string logText;

    //--------- Message ---------//
    logText += "Message: ";
    if (logLevelFlags & kLogLevelFlagInfo) {
        logText += "[INFO] ";
    } else if (logLevelFlags & kLogLevelFlagWarning) {
        logText += "[--- WARNING ---] ";
    } else if (logLevelFlags & kLogLevelFlagError) {
        logText += "[!!!--- ERROR ---!!!] ";
    } else {
        logText += "[NO LEVEL LOG] ";
    }
    logText += message;

    return logText;
}

} // namespace

void InitializeLog(const std::string &filePath, const std::string &projectDir, const LogLevelFlags outputLogLevel) {
    // プロジェクトのルートディレクトリを保存
    MyEngine::projectDir = projectDir;

    // ログファイルのパスを作成
    std::filesystem::create_directory(filePath);
    // 時刻を使ってファイル名を決定
    std::string logFilePath = filePath + '/' + TimeGetString("{:%Y-%m-%d_%H-%M-%S}") + ".log";
    // ファイルを使って書き込み準備
    logStream.open(logFilePath);

    // 出力するログの種類を保存
    MyEngine::outputLogLevel = outputLogLevel;

    // 初期化完了のログとプロジェクトのルートディレクトリと出力するログの種類をログに出力
    logStream << "Log initialized." << std::endl;
    logStream << "Project Directory: \"" << projectDir << "\"" << std::endl;
    if (outputLogLevel & kLogLevelFlagInfo) {
        logStream << "Output Log Level: [INFO]" << std::endl;
    }
    if (outputLogLevel & kLogLevelFlagWarning) {
        logStream << "Output Log Level: [WARNING]" << std::endl;
    }
    if (outputLogLevel & kLogLevelFlagError) {
        logStream << "Output Log Level: [ERROR]" << std::endl;
    }
}

void Log(const std::string &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & outputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = CreateDetailLogText(location);
    logText += CreateLogText(message, logLevelFlags);
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void Log(const std::wstring &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & outputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // 処理を共通化したいので元あるLog関数を使用
    Log(ConvertString(message), logLevelFlags, location);
}

void Log(const std::source_location &message, const LogLevelFlags logLevelFlags, const std::source_location &location) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & outputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = CreateDetailLogText(location);
    logText += CreateLogText(
        "[Location] [File:\"" +
        GetRelativePath(message.file_name()) +
        "\" Function:\"" +
        message.function_name() +
        "\" Line:" +
        std::to_string(message.line()) +
        "]",
        logLevelFlags
    );
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void LogSimple(const std::string &message, const LogLevelFlags logLevelFlags) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & outputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = '\t' + CreateLogText(message, logLevelFlags);
    // ログファイルに書き込み
    logStream << logText << std::endl;
    // デバッグウィンドウに出力
    OutputDebugStringA((logText + '\n').c_str());
}

void LogSimple(const std::wstring &message, const LogLevelFlags logLevelFlags) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & outputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // 処理を共通化したいので元あるLog関数を使用
    LogSimple(ConvertString(message), logLevelFlags);
}

void LogSimple(const std::source_location &message, const LogLevelFlags logLevelFlags) {
    // ログレベルフラグのチェック
    if ((logLevelFlags & outputLogLevel) == 0) {
        // 出力しないログレベルフラグの場合は何もしない
        return;
    }

    // ログテキストを作成
    std::string logText = '\t' + CreateLogText(
        "[Location] [File:\"" +
        GetRelativePath(message.file_name()) +
        "\" Function:\"" +
        message.function_name() +
        "\" Line:" +
        std::to_string(message.line()) +
        "]",
        logLevelFlags
    );
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