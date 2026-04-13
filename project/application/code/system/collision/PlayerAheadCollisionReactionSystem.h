#pragma once

#include "system/ISystem.h"

/// <summary>
/// ProxyColliderの衝突を検知しアニメーションをトリガーする
/// </summary>
class PlayerAheadCollisionReactionSystem
    : public OriGine::ISystem {
public:
    PlayerAheadCollisionReactionSystem();
    ~PlayerAheadCollisionReactionSystem() {}

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
