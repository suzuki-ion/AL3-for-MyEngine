#include "Skydome.h"
#include <cassert>

using namespace KashipanEngine;

Skydome::Skydome(KashipanEngine::Model *model) {
	// NULLポインタチェック
	assert(model);
	// モデルの設定
	model_ = model;
}

void Skydome::Update() {
    // ワールド変換の転送
    worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_);
}