#pragma once
#include "system/ISystem.h"

/// <summary>
/// Playerの速度を 3dUIに適応するクラス
/// </summary>
class PlayerSpeedFor3dUI
    : public OriGine::ISystem {
public:
    PlayerSpeedFor3dUI();
    ~PlayerSpeedFor3dUI() override;

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
