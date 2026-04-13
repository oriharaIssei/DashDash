#pragma once

#include "system/ISystem.h"

/// <summary>
/// 速度補正適用システム
/// </summary>
class ApplySpeedModifiers
    : public OriGine::ISystem {
public:
    ApplySpeedModifiers();
    ~ApplySpeedModifiers() override;

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
