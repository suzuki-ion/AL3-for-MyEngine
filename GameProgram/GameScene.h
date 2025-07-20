#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Math/Camera.h>
#include <3d/DirectionalLight.h>
#include <Common/GridLine.h>

#include "Player.h"
#include "Enemy.h"
#include "CollisionManager.h"
#include "Skydome.h"
#include "Ground.h"

class GameScene {
public:
    // コンストラクタ
    GameScene(Engine *kashipanEngine);
    
	// 更新
	void Update();
	// 描画
	void Draw();

private:
    void CheckAllCollisions();

    // プレイヤー
    std::unique_ptr<Player> player_;
    // 敵
    std::unique_ptr<Enemy> enemy_;
    // 衝突管理
    std::unique_ptr<CollisionManager> collisionManager_;
    // スカイドーム
    std::unique_ptr<Skydome> skydome_;
    // 地面
    std::unique_ptr<Ground> ground_;
    
    // グリッド線
    std::unique_ptr<KashipanEngine::GridLine> gridLine_;
    // カメラ
    std::unique_ptr<KashipanEngine::Camera> camera_;
    // ライト
    KashipanEngine::DirectionalLight light_;
};
