#include <Base/Renderer.h>

#include "Easings.h"
#include "GameScene.h"
#include "CollisionConfig.h"
#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"
#include <algorithm>
#include <array>

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
} // namespace

Enemy::Enemy(Engine *kashipanEngine, GameScene *gameScene,
    const KashipanEngine::Vector3 &startPos, const KashipanEngine::Vector3 &endPos,
    int useEasingNum, float maxEaseTime) {
    // NULLポインタチェック
    sKashipanEngine = kashipanEngine;
    gameScene_ = gameScene;
    // エンジンのレンダラーを取得
    Renderer *renderer = sKashipanEngine->GetRenderer();
    worldTransform_ = std::make_unique<KashipanEngine::WorldTransform>();
    worldTransform_->translate_ = position;
    worldTransform_->TransferMatrix();
    // 初期で横向きにする
    worldTransform_->rotate_.y = std::numbers::pi_v<float> / 2.0f;
    // ワールド変換データの設定
    worldTransform_ = std::make_unique<WorldTransform>();
void Enemy::Update() {
    // 最初の位置と最後の位置と使うイージングの値の設定
    startPos_ = startPos;
    endPos_ = endPos;
    useEasingNum_ = useEasingNum;
    maxEaseTime_ = maxEaseTime;
    CollisionCheck();
    // 旋回処理
    Turn();
    // 歩行アニメーション
    // 弾の時限発動の設定
    timedCall_ = std::make_unique<TimedCall>(
        std::bind(&Enemy::Fire, this), 2.0f, false
    );

    SetCollisionAttribute(kCollisionAttributeEnemy);
    SetCollisionMask(std::bitset<8>(kCollisionAttributeEnemy).flip());
    SetRadius(1.0f);
}

void Enemy::AddTranslate(const KashipanEngine::Vector3 &translate) {
    worldTransform_->translate_ += translate;
}

void Enemy::SetBulletFireEnable(bool enable) {
    if (enable) {
        if (timedCall_ != nullptr) {
            return;
        }
        timedCall_ = std::make_unique<TimedCall>(
            std::bind(&Enemy::Fire, this), 1.0f, true
        );

    } else {
        if (timedCall_ == nullptr) {
            return;
        }
        timedCall_.reset();
    }
}
void Enemy::Move() {
void Enemy::SetPlayerPosition(KashipanEngine::Vector3 playerPosition) {
    playerPosition_ = playerPosition;
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> newState) {
    state_ = std::move(newState);
}

void Enemy::OnCollision() {
    isAlive_ = false;
}

void Enemy::Update() {
    //state_->Update();
    Move();

    // 座標のイージングが終わったら死なせる
    if (currentEaseTimer_ >= maxEaseTime_) {
        isAlive_ = false;
    }
    static const float kWalkMotionAngleEnd = -45.0f;
    // 弾の発射処理
    if (timedCall_ != nullptr) {
        timedCall_->Update();
    }
}
    // 回転アニメーション
void Enemy::Draw() {
    model_->Draw(*worldTransform_);
}
    worldTransform_->rotate_.x = std::numbers::pi_v<float> *degree / 180.0f;
void Enemy::Fire() {
    Vector3 enemyPosition(
        worldTransform_->worldMatrix_.m[3][0],
        worldTransform_->worldMatrix_.m[3][1],
        worldTransform_->worldMatrix_.m[3][2]
    );
    Vector3 kBulletVelocity(playerPosition_ - enemyPosition);
    kBulletVelocity = kBulletVelocity.Normalize();

    std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>(
        sKashipanEngine,
        worldTransform_->translate_,
        TransformNormal(kBulletVelocity, worldTransform_->worldMatrix_),
        5.0f,
        32.0f
    );

    gameScene_->AddEnemyBullet(
        bullet
    );
}

void Enemy::Move() {
    worldTransform_->translate_.x =
        Ease::Auto(currentEaseTimer_, maxEaseTime_, startPos_.x, endPos_.x, useEasingNum_);
    worldTransform_->translate_.y =
        Ease::Auto(currentEaseTimer_, maxEaseTime_, startPos_.y, endPos_.y, useEasingNum_);
    worldTransform_->translate_.z =
        Ease::Auto(currentEaseTimer_, maxEaseTime_, startPos_.z, endPos_.z, useEasingNum_);

    currentEaseTimer_ += Engine::GetDeltaTime();
}        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftTop]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.y = std::max(0.0f, (rect.bottom - worldTransform_->translate_.y) - (Enemy::kHeight / 2.0f));
    // 天井に当たったことを記録する
    collisionMapInfo.isHitUp = true;
}

void Enemy::CollisionCheckDown(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.y >= 0.0f) {
        // 下方向に移動していない場合は当たり判定しない
        return;
    }

    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            KashipanEngine::Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x, worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };

    bool isHit = false;
    // 下辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightBottom]));
    // 下辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.y = std::min(0.0f, (rect.top - worldTransform_->translate_.y) + (Enemy::kHeight / 2.0f));
    collisionMapInfo.isHitGround = true;
}

void Enemy::CollisionCheckLeft(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.x >= 0.0f) {
        // 左方向に移動していない場合は当たり判定しない
        return;
    }

    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            KashipanEngine::Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x, worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };

    bool isHit = false;
    // 左辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftTop]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]));
    // 左辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kLeftBottom]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.x = std::max(0.0f, (rect.right - worldTransform_->translate_.x) - (Enemy::kWidth / 2.0f));
    collisionMapInfo.isHitWall = true;
}

void Enemy::CollisionCheckRight(CollisionMapInfo &collisionMapInfo) {
    if (collisionMapInfo.velocity.x <= 0.0f) {
        // 右方向に移動していない場合は当たり判定しない
        return;
    }
    // 移動後の4つの角の座標
    std::array<KashipanEngine::Vector3, Enemy::kNumCorner> positionNew{};
    for (uint32_t i = 0; i < positionNew.size(); ++i) {
        positionNew[i] = CornerPosition(
            KashipanEngine::Vector3(
                worldTransform_->translate_.x + collisionMapInfo.velocity.x, worldTransform_->translate_.y + collisionMapInfo.velocity.y,
                worldTransform_->translate_.z + collisionMapInfo.velocity.z),
            static_cast<Enemy::Corner>(i));
    };
    bool isHit = false;
    // 右辺のインデックス全てを取得
    std::vector<IndexSet> indexSets;
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightTop]));
    indexSets.push_back(mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightBottom]));
    // 右辺のマップチップを取得し、ブロックに当たっていたらフラグを立てる
    for (const auto &indexSet : indexSets) {
        MapChipType mapChipType = mapChipField_->GetMapChipType(indexSet.x, indexSet.y);
        if (mapChipType != MapChipType::kBlank) {
            isHit = true;
            break;
        }
    }
    if (!isHit) {
        // ブロックに当たっていない場合は何もしない
        return;
    }

    // めり込みを排除する方向に移動量を設定する
    IndexSet indexSet = mapChipField_->GetMapChipIndex(positionNew[Enemy::kRightBottom]);
    // めり込み先ブロックの範囲矩形
    Rect rect = mapChipField_->GetRect(indexSet.x, indexSet.y);
    collisionMapInfo.velocity.x = std::min(0.0f, (rect.left - worldTransform_->translate_.x) + (Enemy::kWidth / 2.0f));
    collisionMapInfo.isHitWall = true;
}

void Enemy::ApplyCollisionResult(const CollisionMapInfo &collisionMapInfo) {
    // 速度を適用する
    worldTransform_->translate_.x += collisionMapInfo.velocity.x;
    worldTransform_->translate_.y += collisionMapInfo.velocity.y;
    worldTransform_->translate_.z += collisionMapInfo.velocity.z;
    // 速度を反映する
    velocity_.x = collisionMapInfo.velocity.x;
    velocity_.y = collisionMapInfo.velocity.y;
    velocity_.z = collisionMapInfo.velocity.z;
}

void Enemy::OnHitCeiling(const CollisionMapInfo &collisionMapInfo) {
    // 天井に当たった場合は上方向の速度を0にする
    if (collisionMapInfo.isHitUp) {
        velocity_.y = 0.0f;
    }
}

void Enemy::OnHitWall(const CollisionMapInfo &collisionMapInfo) {
    const float kAttenuationWall = 0.1f;

    // 壁接触による減速
    if (collisionMapInfo.isHitWall) {
        velocity_.x *= (1.0f - kAttenuationWall);
    }
}

void Enemy::SwitchOnGroundState(CollisionMapInfo &collisionMapInfo) {
    static const float kAttenuationLanding = 0.1f;
    static const float kDownPosPlus = 2.0f;

    if (onGround_) {
        // 接地状態の処理
        if (velocity_.y > 0.0f) {
            onGround_ = false;
        } else {
            if (!collisionMapInfo.isHitGround) {
                // 落下開始
                onGround_ = false;
            }
        }

    } else {
        // 空中状態の処理
        if (collisionMapInfo.isHitGround) {
            // 着地状態に切り替える
            onGround_ = true;
            // 着地時にX速度を減衰
            velocity_.x *= (1.0f - kAttenuationLanding);
            // Y速度をリセット
            velocity_.y = 0.0f;
        }
    }
}

void Enemy::Turn() {
    // 左右の自キャラ角度テーブル
    static const float destinationRotationYTable[] = {
        std::numbers::pi_v<float> / 2.0f,
        std::numbers::pi_v<float> *3.0f / 2.0f,
    };
    // 旋回時間<秒>
    const float kTimeTurn = 0.3f;

    if (turnTimer_ <= kTimeTurn) {
        // 旋回タイマーを1/60秒分カウントする
        turnTimer_ += 1.0f / 60.0f;

        // 状態に応じた角度を取得する
        float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
        // 自キャラの角度を設定する
        worldTransform_->rotate_.y = Ease::OutCubic(turnTimer_ / kTimeTurn, turnFirstRotationY_, destinationRotationY);
    }
}
