#pragma once

#include "system/ISystem.h"

/// <summary>
/// プレイヤーを追従するシステム
/// </summary>
class PlayerFollowSystem
    : public OriGine::ISystem {
public:
    PlayerFollowSystem();
    ~PlayerFollowSystem() override = default;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
