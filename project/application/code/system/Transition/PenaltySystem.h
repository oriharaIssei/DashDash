#pragma once
#include "system/ISystem.h"

/// stl
#include <memory>

/// system
#include "system/initialize/CreateSpriteFromTimer.h"

/// math
#include "math/Matrix4x4.h"

/// <summary>
/// ペナルティを適応するシステム
/// </summary>
class PenaltySystem
    : public OriGine::ISystem {
public:
    PenaltySystem();
    ~PenaltySystem();
    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

protected:
    std::unique_ptr<CreateSpriteFromTimer> createSpriteFromTimerSystem_ = nullptr;
    OriGine::Matrix4x4 viewPortMat_; // ビューポート行列
};
