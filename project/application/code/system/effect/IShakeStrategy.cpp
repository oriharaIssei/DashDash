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

		Vec2f st(
			time,
			time * EffectConfig::CameraShake::kNoiseCoefficient
		);

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

		float acc = -(omega * omega) * _source.GetSpringPosition()[i]
			- 2.0f * zeta * omega * _source.GetSpringVelocity()[i];
		_source.GetSpringVelocity()[i] += acc * _deltaTime;
		_source.GetSpringPosition()[i] += _source.GetSpringVelocity()[i] * _deltaTime;

		offset[i] = _source.GetSpringPosition()[i];
	}
	if(!_source.IsLoop() && _source.GetSpringPosition().length() < kEpsilon && _source.GetSpringVelocity().length() < kEpsilon){
		_source.SetActive(false);
	}
	return offset;
}
