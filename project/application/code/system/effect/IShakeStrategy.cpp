#include "IShakeStrategy.h"

#include "EffectConfig.h"
#include "Math/Noise.h"
#include "Math/MathEnv.h"

using namespace OriGine;

//=============================================================================
// ShakeStrategy implementations
//=============================================================================

Vec3f SinCurveShakeStrategy::Calculate(CameraShakeSourceComponent& _source,float /*_deltaTime*/){
	Vec3f offset(0.0f,0.0f,0.0f);
	for(size_t i = 0; i < 3; ++i){
		offset[i] = sinf(_source.GetElapsedTime() * _source.GetAxisParameters()[i].frequency) * _source.GetAxisParameters()[i].amplitude;
	}
	return offset;
}

Vec3f NoiseShakeStrategy::Calculate(CameraShakeSourceComponent& _source,float /*_deltaTime*/){
	Vec3f offset(0.0f,0.0f,0.0f);
	for(size_t i = 0; i < 3; ++i){
		float time = _source.GetElapsedTime() * _source.GetAxisParameters()[i].frequency;

		// 軸ごとにサンプリング座標をずらし、各軸の揺れが同期しないようにする
		Vec2f st(
			time,
			time * EffectConfig::CameraShake::kNoiseCoefficient
		);

		// Fbmノイズは[0,1]付近の値を取るため、kNoiseOffsetを引いて0を中心に振れるようにする
		float f = FbmNoise::Fbm(st) - EffectConfig::CameraShake::kNoiseOffset;

		offset[i] = f * _source.GetAxisParameters()[i].amplitude;
	}
	return offset;
}

Vec3f SpringShakeStrategy::Calculate(CameraShakeSourceComponent& _source,float _deltaTime){
	Vec3f offset(0.0f,0.0f,0.0f);
	for(size_t i = 0; i < 3; ++i){
		float omega = kTau * _source.GetAxisParameters()[i].frequency;
		float zeta  = _source.GetAxisParameters()[i].dampingRatio;

		// バネ-ダンパーモデルによる加速度計算（フックの法則＋速度に比例した減衰）
		float acc = -(omega * omega) * _source.GetSpringPosition()[i]
			- 2.0f * zeta * omega * _source.GetSpringVelocity()[i];
		// 速度・位置をオイラー法で積分して更新
		_source.GetSpringVelocity()[i] += acc * _deltaTime;
		_source.GetSpringPosition()[i] += _source.GetSpringVelocity()[i] * _deltaTime;

		offset[i] = _source.GetSpringPosition()[i];
	}
	// ループ再生でなく、位置・速度ともに十分小さくなったら振動が収まったとみなして非アクティブ化
	if(!_source.IsLoop() && _source.GetSpringPosition().length() < kEpsilon && _source.GetSpringVelocity().length() < kEpsilon){
		_source.SetActive(false);
	}
	return offset;
}
