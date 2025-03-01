#pragma once

/// @brief エンジンのシステムクラス
class EngineSystem {
public:
    EngineSystem(const EngineSystem &) = delete;
    EngineSystem &operator=(const EngineSystem &) = delete;
    EngineSystem(EngineSystem &&) = delete;
    EngineSystem &operator=(EngineSystem &&) = delete;
    
    /// @brief インスタンス取得
    /// @return エンジンシステムクラスのインスタンス
    static EngineSystem *GetInstance() {
        static EngineSystem instance;
        return &instance;
    }
    
private:
    EngineSystem() = default;
    ~EngineSystem() = default;
};

