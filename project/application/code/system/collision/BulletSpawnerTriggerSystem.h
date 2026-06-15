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
    ICollisionTriggerComponent* GetTrigger(const OriGine::EntityHandle& _handle) override;
    void ApplyActivate(const OriGine::EntityHandle& _targetHandle) override;
    void ApplyDeactivate(const OriGine::EntityHandle& _targetHandle) override;
};
