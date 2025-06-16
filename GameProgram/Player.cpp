#include <Base/Renderer.h>
#include <Base/Input.h>

#include "Player.h"

using namespace KashipanEngine;

Player::Player(Engine *kashipanEngine) {
    kashipanEngine_ = kashipanEngine;
    // エンジンのレンダラーを取得
    Renderer *renderer = kashipanEngine_->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Player", "player.obj");
    model_->SetRenderer(renderer);

    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();
    worldTransform_->translate_.z = 64.0f;
}

void Player::Update() {
    Move();
}

void Player::Draw() {
    // モデルを描画
    model_->Draw(*worldTransform_.get());
}

void Player::Move() {
    // 移動制限
    const float kMoveLimitX = worldTransform_->translate_.z * 16.0f / 48.0f;
    const float kMoveLimitY = worldTransform_->translate_.z * 9.0f  / 48.0f;

    // 移動速度上限
    const float kSpeedLimit = 1.0f;

    // 上下左右キーで移動
    if (Input::IsKeyDown(DIK_LEFT) == Input::IsKeyDown(DIK_RIGHT)) {
        // 入力状態が一致してる場合は減速
        velocity_.x *= (1.0f - kMoveSpeed);

    } else if (Input::IsKeyDown(DIK_LEFT) || Input::IsKeyDown(DIK_RIGHT)) {
        if (Input::IsKeyDown(DIK_LEFT)) {
            velocity_.x -= kMoveSpeed;
        }
        if (Input::IsKeyDown(DIK_RIGHT)) {
            velocity_.x += kMoveSpeed;
        }
        velocity_.x = std::clamp(velocity_.x, -kSpeedLimit, kSpeedLimit);
    }

    if (Input::IsKeyDown(DIK_UP) == Input::IsKeyDown(DIK_DOWN)) {
        // 入力状態が一致してる場合は減速
        velocity_.y *= (1.0f - kMoveSpeed);

    } else if (Input::IsKeyDown(DIK_UP) || Input::IsKeyDown(DIK_DOWN)) {
        if (Input::IsKeyDown(DIK_UP)) {
            velocity_.y += kMoveSpeed;
        }
        if (Input::IsKeyDown(DIK_DOWN)) {
            velocity_.y -= kMoveSpeed;
        }
        velocity_.y = std::clamp(velocity_.y, -kSpeedLimit, kSpeedLimit);
    }

    worldTransform_->translate_ += velocity_;
    
    // 移動制限範囲を超えたら制限範囲内に戻して速度を0にする
    if (std::clamp(worldTransform_->translate_.x, -kMoveLimitX, kMoveLimitX) != worldTransform_->translate_.x) {
        worldTransform_->translate_.x = std::clamp(worldTransform_->translate_.x, -kMoveLimitX, kMoveLimitX);
        velocity_.x = 0.0f;
    }
    if (std::clamp(worldTransform_->translate_.y, -kMoveLimitY, kMoveLimitY) != worldTransform_->translate_.y) {
        worldTransform_->translate_.y = std::clamp(worldTransform_->translate_.y, -kMoveLimitY, kMoveLimitY);
        velocity_.y = 0.0f;
    }
}