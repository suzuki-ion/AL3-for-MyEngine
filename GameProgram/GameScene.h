<<<<<<< HEAD
﻿#pragma once
=======
#pragma once
>>>>>>> chaptor3のマージ
#include <sstream>
#include <KashipanEngine.h>
#include <Objects.h>
#include <Math/Camera.h>
#include <3d/DirectionalLight.h>
#include <Common/GridLine.h>

#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "CollisionManager.h"
#include "Skydome.h"
#include "Ground.h"
#include "RailCameraController.h"
#include "Reticle2D.h"
#include "LockOn.h"

enum class PerspectiveType {
    ThirdPerson,    // 三人称視点
    FirstPerson,    // 一人称視点
};

class GameScene {
public:
    // コンストラクタ
    GameScene(Engine *kashipanEngine);
    
    // プレイヤーの弾の追加
    void AddPlayerBullet(std::unique_ptr<PlayerBullet> &bullet) {
        playerBullets_.push_back(std::move(bullet));
    }
    // 敵の弾の追加
    void AddEnemyBullet(std::unique_ptr<EnemyBullet> &bullet) {
        enemyBullets_.push_back(std::move(bullet));
    }

	// 更新
	void Update();
	// 描画
	void Draw();

private:
    void CheckAllCollisions();
    void LoadEnemyPopData();
    void UpdateEnemyPopCommands();
    void PopEnemy(const KashipanEngine::Vector3 &startPos, const KashipanEngine::Vector3 &endPos,
        int useEasingNum, float easeMaxTime);

    // プレイヤー
    std::unique_ptr<Player> player_;
    // 敵
    std::list<std::unique_ptr<Enemy>> enemies_;
    // 衝突管理
    std::unique_ptr<CollisionManager> collisionManager_;
    // スカイドーム
    std::unique_ptr<Skydome> skydome_;
    // 地面
    std::unique_ptr<Ground> ground_;
    // 照準
    //std::unique_ptr<Reticle2D> reticle_;
    // ロックオン
    std::unique_ptr<LockOn> lockOn_;

    // プレイヤーの弾
    std::list<std::unique_ptr<PlayerBullet>> playerBullets_;
    // 敵の弾
    std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
    // 敵の弾のモデル
    std::unique_ptr<KashipanEngine::Model> enemyBulletModel_;

    // カメラコントローラー
    std::unique_ptr<RailCameraController> railCameraController_;
    
    // グリッド線
    std::unique_ptr<KashipanEngine::GridLine> gridLine_;
    // 三人称カメラ
    std::unique_ptr<KashipanEngine::Camera> thirdPersonCamera_;
    // 一人称カメラ
    std::unique_ptr<KashipanEngine::Camera> firstPersonCamera_;
    // ライト
    KashipanEngine::DirectionalLight light_;

    // 敵発生コマンド
    std::stringstream enemyPopCommands_;
    // 敵発生待ち時間
    int32_t enemyPopWaitTimer_ = 0;
    // 敵発生待ち時間フラグ
    bool isEnemyPopWait_ = false;

    // 視点タイプ
    PerspectiveType perspectiveType_ = PerspectiveType::ThirdPerson;
};
