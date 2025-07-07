#pragma once
#include <KashipanEngine.h>
#include <Objects.h>
#include <Math/Camera.h>
#include <3d/DirectionalLight.h>

#include "Player.h"
#include "Enemy.h"

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

    // カメラ
    std::unique_ptr<KashipanEngine::Camera> camera_;

    // ライト
    KashipanEngine::DirectionalLight light_;
};
