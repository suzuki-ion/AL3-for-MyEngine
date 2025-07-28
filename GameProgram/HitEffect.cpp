#include "HitEffect.h"
#include "Easings.h"
#include <cassert>
#include <random>
#include <numbers>

using namespace KashipanEngine;

std::unique_ptr<HitEffect> HitEffect::Create(const KashipanEngine::Vector3 &pos) {
	auto hitEffect = std::make_unique<HitEffect>();
	hitEffect->Initialize(pos);
    return hitEffect;
}

void HitEffect::Initialize(const KashipanEngine::Vector3 &pos) {
	for (auto &model : model_->GetModels()) {
		model.GetStatePtr().material->color.w = 255.0f;
	}

	// 乱数の初期化
	std::random_device seedGenerator;
	std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	// 指定範囲の乱数生成器
	std::uniform_real_distribution<float> rotationDistribution(
		-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	circleWorldTransforms_.translate_ = pos;
	for (auto &worldTransform : ellipseWorldTransforms_) {
		float randomRotate = rotationDistribution(randomEngine);
		worldTransform.scale_ = { ellipseWidth_, ellipseHeight_ , 1.0f };
		worldTransform.rotate_ = { 0.0f, 0.0f, randomRotate };
		worldTransform.translate_ = pos;
	}

	state_ = State::kScaleUp;
}

void HitEffect::Update() {
	// 終了状態なら何もせず終わる
	if (state_ == State::kFinished) {
		return;
	}

	// 拡大の速さ (秒)
	const float scaleUpTime = 0.5f;
	// フェードアウトの速さ (秒)
	const float fadeOutTime = 0.5f;

	// 時間計算用
	float t = 0.0f;
	// イージングの値保存用
	float easing = 0.0f;
	switch (state_) {
		case HitEffect::State::kScaleUp:
			t = counter_ / scaleUpTime;
			easing = Ease::OutCubic(t, 0.0f, 1.0f);
			circleWorldTransforms_.scale_.x = easing;
			circleWorldTransforms_.scale_.y = easing;
			for (auto &worldTransform : ellipseWorldTransforms_) {
				worldTransform.scale_.x = ellipseWidth_ * easing;
				worldTransform.scale_.y = ellipseHeight_ * easing;
			}

			// カウンターが指定の時間にまで達したら状態遷移
			if (counter_ >= scaleUpTime) {
				state_ = State::kFadeOut;
				counter_ = 0.0f;
				return;
			}
			break;

		case HitEffect::State::kFadeOut:
			t = counter_ / fadeOutTime;
			easing = Ease::OutCubic(t, 255.0f, 0.0f);

			for (auto &model : model_->GetModels()) {
				model.GetStatePtr().material->color.w = easing;
			}

			// カウンターが指定の時間にまで達したら終了
			if (counter_ >= fadeOutTime) {
				state_ = State::kFinished;
				return;
			}
			break;
	}

	counter_ += kashipanEngine_->GetDeltaTime();
}

void HitEffect::Draw() {
	for (auto &worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform);
	}
	model_->Draw(circleWorldTransforms_);
}