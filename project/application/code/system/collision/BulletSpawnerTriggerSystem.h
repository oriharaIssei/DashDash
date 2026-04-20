#pragma once

#include "system/collision/ICollisionTriggerSystem.h"

/// <summary>
/// BulletSpawnerTrigger を持つ Entity の Collider が衝突 (Enter) したとき、
/// ターゲット Entity の BulletSpawner を Activate / Deactivate するシステム。
/// </summary>
class BulletSpawnerTriggerSystem
    : public ICollisionTriggerSystem {
public:
    BulletSpawnerTriggerSystem()           = default;
    ~BulletSpawnerTriggerSystem() override = default;

protected:
    ICollisionTriggerComponent* GetTrigger(OriGine::EntityHandle _handle) override;
    void ApplyActivate(OriGine::EntityHandle _targetHandle) override;
    void ApplyDeactivate(OriGine::EntityHandle _targetHandle) override;
};
