#pragma once

namespace MyEngine {

/// @brief リソースリークチェック用構造体
struct D3DResourceLeakChecker {
    ~D3DResourceLeakChecker();
};

} // namespace MyEngine