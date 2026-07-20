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
    /// <summary>
    /// ペナルティ状態のPlayerからペナルティ時間分ギアレベルを下げ、
    /// Timerへペナルティ時間を加算したうえで、ペナルティ演出用のタイマー表示Spriteを生成する。
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

protected:
    // ペナルティ時間表示用Spriteの生成に使用するサブシステム
    std::unique_ptr<CreateSpriteFromTimer> createSpriteFromTimerSystem_ = nullptr;
    OriGine::Matrix4x4 viewPortMat_; // ビューポート行列
};
