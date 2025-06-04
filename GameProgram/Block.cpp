#include "Block.h"
#include "MapChipField.h"
#include <cassert>

using namespace KashipanEngine;

extern const uint32_t kNumBlockVertical;
extern const uint32_t kNumBlockHorizontal;
extern const float kBlockWidth;
extern const float kBlockHeight;

Block::Block(KashipanEngine::Model *model) {
	// NULLポインタチェック
	assert(model);

	// モデルの設定
	model_ = model;
	
	// 要素数を変更する
	worldTransforms_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransforms_[i].resize(kNumBlockHorizontal);
	}
}

Block::~Block() {
    // ワールドトランスフォームのリセット
    ResetWorldTransforms();
}

void Block::SetMapChipFieldBlocks(MapChipField* mapChipField) {
	// NULLポインタチェック
	assert(mapChipField);

    // 一度ワールドトランスフォームをリセット
    ResetWorldTransforms();

	// 要素数を変更する
	worldTransforms_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		worldTransforms_[i].resize(kNumBlockHorizontal);
	}
	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVertical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField->GetMapChipType(j, i) == MapChipType::kBlank) {
				continue;
			}
            worldTransforms_[i][j] = new KashipanEngine::WorldTransform();
			worldTransforms_[i][j]->translate_ = mapChipField->GetMapChipPosition(j, i);
		}
	}
}

void Block::Update() {
    // ワールド行列の転送
    for (auto &worldTransformBlockLine : worldTransforms_) {
        for (auto &worldTransformBlock : worldTransformBlockLine) {
            if (!worldTransformBlock) {
                continue;
            }
            worldTransformBlock->TransferMatrix();
        }
    }
}

void Block::Draw() {
	// 3Dモデルを描画
	for (auto& worldTransformBlockLine : worldTransforms_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
            model_->Draw(*worldTransformBlock);
		}
	}
}

void Block::ResetWorldTransforms() {
    // ワールドトランスフォームのリセット
    for (auto &worldTransformBlockLine : worldTransforms_) {
        for (auto &worldTransformBlock : worldTransformBlockLine) {
            if (worldTransformBlock) {
                delete worldTransformBlock;
            }
        }
    }
    worldTransforms_.clear();
}
