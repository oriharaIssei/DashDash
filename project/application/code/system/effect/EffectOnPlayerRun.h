#pragma once
#include "system/ISystem.h"

/// <summary>
/// Playerが走っているときにエフェクトを発生させるシステム
/// </summary>
class EffectOnPlayerRun
    : public OriGine::ISystem {
public:
    EffectOnPlayerRun();
    ~EffectOnPlayerRun();

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _entity) override;

private:
    const float kMaxIntensity_         = 3.8f;
    const float kThresholdSpeedForSpeedWave_ = 52.f; // スピードウェーブエフェクトを発生させる速度閾値

};
