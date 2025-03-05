#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

/// @brief 自作エンジンクラス
class Engine final {
public:
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;
    Engine(Engine &&) = delete;
    Engine &operator=(Engine &&) = delete;

    /// @brief インスタンス取得
    /// @return エンジンクラスのインスタンス
    static Engine *GetInstance() {
        static Engine instance;
        return &instance;
    }

    /// @brief エンジン初期化
    void Initialize(const char *title, int width = 1280, int height = 720, bool enableDebugLayer = true);

    /// @brief エンジン終了処理
    void Finalize();

    /// @brief フレーム開始処理
    void BeginFrame();

    /// @brief フレーム終了処理
    void EndFrame();

    /// @brief テスト用の描画関数
    void DrawTest();
    
    /// @brief ウィンドウハンドル取得
    HWND GetWindowHandle() const;

    /// @brief クライアントサイズの横幅取得
    /// @return クライアントサイズの横幅
    int32_t GetClientWidth() const;
    
    /// @brief クライアントサイズの縦幅取得
    /// @return クライアントサイズの縦幅
    int32_t GetClientHeight() const;
    
    /// @brief メッセージ処理
    /// @return メッセージ処理結果。-1の場合は終了
    int ProccessMessage();

private:
    Engine() = default;
    ~Engine() = default;

    /// @brief 終了処理チェック用構造体
    struct FinalizeChecker {
        ~FinalizeChecker();
    };
    /// @brief 終了処理チェック用変数
    FinalizeChecker finalizeCheck_;
};