#pragma once
#include <Windows.h>

namespace MyEngine {

/// @brief CrashHandlerの登録
/// @param exception 例外ポインタ
/// @return 例外コード
LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception);

} // namespace MyEngine