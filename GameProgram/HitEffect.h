#pragma once
#include <KashipanEngine.h>
#include <array>
#include "Objects.h"

class HitEffect {
public:
    enum class State {
        kUnknown,   // 未定義
        kScaleUp,   // 拡大
        kFadeOut,   // フェードアウト
        kFinished   // 終了
    };

    static void SetGameEngine(Engine *kashipanEngine) {
        kashipanEngine_ = kashipanEngine;
    }
    static void SetModel(KashipanEngine::Model *model) {
        model_ = model;
    }
    static std::unique_ptr<HitEffect> Create(const KashipanEngine::Vector3 &pos);

    void Initialize(const KashipanEngine::Vector3 &pos);

    void Update();

    void Draw();

    // 終了状態かどうかの取得
    bool IsFinished() const { return state_ == State::kFinished; }

private:
    // ゲームエンジン
    static inline Engine *kashipanEngine_ = nullptr;
    // モデル (借りてくる用)
    static inline KashipanEngine::Model *model_ = nullptr;

    // 楕円の横幅
    static inline float ellipseWidth_ = 2.0f;
    // 楕円の縦幅
    static inline float ellipseHeight_ = 0.1f;

    // 円のワールド変換データ
    KashipanEngine::WorldTransform circleWorldTransforms_;
    // 楕円のワールド変換データ
    std::array<KashipanEngine::WorldTransform, 2> ellipseWorldTransforms_;

    // 状態
    State state_ = State::kUnknown;
    // 経過時間カウント
    float counter_ = 0.0f;

};