#pragma once
#include "system/ISystem.h"

/// <summary>
/// Timerの数値をSpriteに適用するシステム
/// </summary>
class TimerForSprite
    : public OriGine::ISystem {
public:
    TimerForSprite();
    ~TimerForSprite() override;

    virtual void Initialize();
    virtual void Finalize();

protected:
    virtual void UpdateEntity(OriGine::EntityHandle _handle) override;
};
