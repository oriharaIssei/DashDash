#pragma once
#include "system/ISystem.h"

struct SpeedFor3dUIComponent;

/// <summary>
/// Timerの数字をSpriteとして表示するためにSpriteを生成するシステム
/// </summary>
class CreatePlaneFromSpeed
    : public OriGine::ISystem {
public:
    CreatePlaneFromSpeed();
    ~CreatePlaneFromSpeed() override;
    void Initialize();
    // virtual void Update();
    // virtual void Edit();
    void Finalize();

    void UpdateEntity(OriGine::EntityHandle _handle) override;
    void CreatePlanesFromComponent(OriGine::EntityHandle _handle, SpeedFor3dUIComponent* _speedFor3dUI);
};
