#pragma once
#include "system/ISystem.h"

struct TimerForSpriteComponent;

/// <summary>
/// Timerの数字をSpriteとして表示するためにSpriteを生成するシステム
/// </summary>
class CreateSpriteFromTimer
    : public OriGine::ISystem {
public:
    CreateSpriteFromTimer();
    ~CreateSpriteFromTimer() override;
    void Initialize();
    // virtual void Update();
    // virtual void Edit();
    void Finalize();

    void UpdateEntity(OriGine::EntityHandle _handle) override;
    void CreateSprites(OriGine::EntityHandle _handle, TimerForSpriteComponent* _forSpriteComp);
};
