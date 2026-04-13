#pragma once

#include "system/ISystem.h"

/// <summary>
/// 衝突した相手に強制移動を適応するシステム
/// </summary>
class VelocityOverrideTriggerSystem
    : public OriGine::ISystem {
public:
    VelocityOverrideTriggerSystem();
    ~VelocityOverrideTriggerSystem() override;

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
