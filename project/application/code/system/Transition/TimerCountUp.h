#pragma once

#include "system/ISystem.h"

/// <summary>
/// タイマーの数値をカウントアップするシステム
/// </summary>
class TimerCountUp
    : public OriGine::ISystem {
public:
    TimerCountUp();
    ~TimerCountUp() override;

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
