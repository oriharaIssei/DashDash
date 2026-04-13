#pragma once
#include "system/ISystem.h"

/// <summary>
/// 時間制限によるゲームオーバー判定を行うシステム
/// </summary>
class TimeLimitJudgeSystem
    : public OriGine::ISystem {
public:
    TimeLimitJudgeSystem();
    ~TimeLimitJudgeSystem() override;
    void Finalize() override;
    void Initialize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
