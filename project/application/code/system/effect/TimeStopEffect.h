#pragma once

#include "system/ISystem.h"

/// <summary>
/// 時間停止エフェクト
/// </summary>
class TimeStopEffect
    : public OriGine::ISystem {
public:
    TimeStopEffect();
    ~TimeStopEffect() override = default;
    void Initialize() override;
    void Finalize() override;

private:
    void Update() override;
};
