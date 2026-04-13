#pragma once

#include "system/ISystem.h"

/// <summary>
/// BulletSpawnerTrigger を持つ Entity の Collider が衝突 (Enter) したとき、
/// ターゲット Entity の BulletSpawner を Activate / Deactivate するシステム。
/// </summary>
class BulletSpawnerTriggerSystem
    : public OriGine::ISystem {
public:
    BulletSpawnerTriggerSystem() : ISystem(OriGine::SystemCategory::Collision) {}
    ~BulletSpawnerTriggerSystem() override = default;

    void Initialize() override {}
    void Finalize() override {}

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
