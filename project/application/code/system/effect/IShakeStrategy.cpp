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
		offset[i] = sinf(_source.elapsedTime * _source.axisParameters[i].frequency) * _source.axisParameters[i].amplitude;
	}
	return offset;
}

Vec3f NoiseShakeStrategy::Calculate(CameraShakeSourceComponent& _source,float /*_deltaTime*/){
	Vec3f offset(0.0f,0.0f,0.0f);
	for(size_t i = 0; i < 3; ++i){
		float time = _source.elapsedTime * _source.axisParameters[i].frequency;

		Vec2f st(
			time,
			time * EffectConfig::CameraShake::kNoiseCoefficient
		);

		float f = FbmNoise::Fbm(st) - EffectConfig::CameraShake::kNoiseOffset;

		offset[i] = f * _source.axisParameters[i].amplitude;
	}
	return offset;
}

Vec3f SpringShakeStrategy::Calculate(CameraShakeSourceComponent& _source,float _deltaTime){
	Vec3f offset(0.0f,0.0f,0.0f);
	for(size_t i = 0; i < 3; ++i){
		float omega = kTau * _source.axisParameters[i].frequency;
		float zeta  = _source.axisParameters[i].dampingRatio;

		float acc = -(omega * omega) * _source.springPosition[i]
			- 2.0f * zeta * omega * _source.springVelocity[i];
		_source.springVelocity[i] += acc * _deltaTime;
		_source.springPosition[i] += _source.springVelocity[i] * _deltaTime;

		offset[i] = _source.springPosition[i];
	}
	if(!_source.isLoop && _source.springPosition.length() < kEpsilon && _source.springVelocity.length() < kEpsilon){
		_source.isActive = false;
	}
	return offset;
}
