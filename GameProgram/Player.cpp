#include <Base/Renderer.h>
#include <Base/Input.h>
#include <Math/Camera.h>
#include <numbers>

#include "CollisionConfig.h"
#include "KeyConfig.h"
#include "Player.h"

using namespace KashipanEngine;

namespace {

// エンジンへのポインタ
Engine *sKashipanEngine = nullptr;

Vector3 TransformNormal(const Vector3 &v, const Matrix4x4 &m) {
    // ベクトルを変換するための行列を適用
    Vector3 result;
    result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
    result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
    result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
    return result;
}

}

Player::Player(Engine *kashipanEngine, Camera *camera) {
    sKashipanEngine = kashipanEngine;
    camera_ = camera;
    // エンジンのレンダラーを取得
    Renderer *renderer = sKashipanEngine->GetRenderer();

    // モデルデータを作成
    model_ = std::make_unique<Model>("Resources/Player", "player.obj");
    model_->SetRenderer(renderer);

    bulletModel_ = std::make_unique<Model>("Resources/Bullet", "bullet.obj");
    bulletModel_->SetRenderer(renderer);

    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();

    SetCollisionAttribute(kCollisionAttributePlayer);
    SetCollisionMask(std::bitset<8>(kCollisionAttributePlayer).flip());
    SetRadius(1.0f);
}

void Player::OnCollision() {
}

void Player::Update() {
    InputMove();
    InputRotate();
    Move();
    Rotate();
    LimitPosition();
    Attack();

    // 弾の更新
    for (auto &bullet : bullets_) {
        bullet->Update();
    }
    // 弾の削除処理
    bullets_.remove_if([](const std::unique_ptr<PlayerBullet> &bullet) {
        return !bullet->IsAlive();
    });
}

void Player::Draw() {
    // モデルを描画
    model_->Draw(*worldTransform_.get());

    // 弾を描画
    for (auto &bullet : bullets_) {
        bullet->Draw();
    }
}

void Player::InputMove() {
    if (IsAction(Action::kMoveLeft) == IsAction(Action::kMoveRight)) {
        moveDirectionLR_ = MoveDirectionLR::kMoveNone;
    } else {
        if (IsAction(Action::kMoveLeft)) {
            moveDirectionLR_ = MoveDirectionLR::kMoveLeft;
        } else if (IsAction(Action::kMoveRight)) {
            moveDirectionLR_ = MoveDirectionLR::kMoveRight;
        }
    }

    if (IsAction(Action::kMoveUp) == IsAction(Action::kMoveDown)) {
        moveDirectionUD_ = MoveDirectionUD::kMoveNone;
    } else {
        if (IsAction(Action::kMoveUp)) {
            moveDirectionUD_ = MoveDirectionUD::kMoveUp;
        } else if (IsAction(Action::kMoveDown)) {
            moveDirectionUD_ = MoveDirectionUD::kMoveDown;
        }
    }
}

void Player::InputRotate() {
    // 調整回転角[ラジアン]
    const float kAdjustmentRotate = 10.0f * std::numbers::pi_v<float> / 180.0f;

    // 回転角をリセット
    rotateVector_ = Vector3(0.0f);

    if (IsAction(Action::kRotateLeft)) {
        rotateVector_.y = -kAdjustmentRotate;
        rotateVector_.z = kAdjustmentRotate;
    } else if (IsAction(Action::kRotateRight)) {
        rotateVector_.y = kAdjustmentRotate;
        rotateVector_.z = -kAdjustmentRotate;
    }

    if (IsAction(Action::kRotateUp)) {
        rotateVector_.x = -kAdjustmentRotate;
    } else if (IsAction(Action::kRotateDown)) {
        rotateVector_.x = kAdjustmentRotate;
    }
}

void Player::Move() {
    // 移動速度上限
    const float kSpeedLimit = 30.0f * Engine::GetDeltaTime();
    // 1フレームあたりの移動速度
    const float kMoveSpeed = kMoveSpeedSecond * Engine::GetDeltaTime();

    // 横方向
    if (moveDirectionLR_ == MoveDirectionLR::kMoveLeft) {
        velocity_.x -= kMoveSpeed;
    } else if (moveDirectionLR_ == MoveDirectionLR::kMoveRight) {
        velocity_.x += kMoveSpeed;
    } else {
        velocity_.x *= (1.0f - kMoveSpeed);
    }
    velocity_.x = std::clamp(velocity_.x, -kSpeedLimit, kSpeedLimit);

    // 縦方向
    if (moveDirectionUD_ == MoveDirectionUD::kMoveUp) {
        velocity_.y += kMoveSpeed;
    } else if (moveDirectionUD_ == MoveDirectionUD::kMoveDown) {
        velocity_.y -= kMoveSpeed;
    } else {
        velocity_.y *= (1.0f - kMoveSpeed);
    }
    velocity_.y = std::clamp(velocity_.y, -kSpeedLimit, kSpeedLimit);

    worldTransform_->translate_ += velocity_;

    // ワールド変換データを更新
    worldTransform_->TransferMatrix();
}

void Player::Rotate() {
    // 調整回転角[ラジアン]
    const float kAdjustmentRotate = 10.0f * std::numbers::pi_v<float> / 180.0f;
    // 1フレームあたりの回転速度
    const float kRotateSpeed = kRotateSpeedSecond * Engine::GetDeltaTime();

    // 移動方向に応じて回転を調整
    if (moveDirectionLR_ == MoveDirectionLR::kMoveLeft) {
        rotateVector_.y -= kAdjustmentRotate;
        rotateVector_.z += kAdjustmentRotate;
    } else if (moveDirectionLR_ == MoveDirectionLR::kMoveRight) {
        rotateVector_.y += kAdjustmentRotate;
        rotateVector_.z -= kAdjustmentRotate;
    }

    if (moveDirectionUD_ == MoveDirectionUD::kMoveUp) {
        rotateVector_.x -= kAdjustmentRotate;
    } else if (moveDirectionUD_ == MoveDirectionUD::kMoveDown) {
        rotateVector_.x += kAdjustmentRotate;
    }

    // 回転角に向かってワールド変換データを回転
    worldTransform_->rotate_ += (rotateVector_ - worldTransform_->rotate_) * kRotateSpeed;
}

void Player::LimitPosition() {
    const float length = (camera_->GetTranslate() - worldTransform_->translate_).Length();
    const float kMoveLimitX = length * 16.0f / 48.0f;
    const float kMoveLimitY = length * 9.0f / 48.0f;
    
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

void Player::Attack() {
    // 弾の発射
    if (IsAction(Action::kShootBullet)) {
        ShootBullet();
    }
}

void Player::ShootBullet() {
    const Vector3 kBulletVelocity(0.0f, 0.0f, 6.0f);

    bullets_.push_back(
        std::make_unique<PlayerBullet>(
            sKashipanEngine,
            bulletModel_.get(),
            worldTransform_->translate_,
            TransformNormal(kBulletVelocity, worldTransform_->worldMatrix_),
            5.0f
        )
    );
}
