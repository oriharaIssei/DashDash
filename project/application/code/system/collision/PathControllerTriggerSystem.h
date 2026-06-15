#pragma once

#include "system/collision/ICollisionTriggerSystem.h"

/// <summary>
/// PathControllerTrigger を持つ Entity の Collider が衝突 (Enter) したとき、
/// ターゲット Entity の PathController を Activate / Deactivate するシステム。
/// </summary>
class PathControllerTriggerSystem
    : public ICollisionTriggerSystem {
public:
    PathControllerTriggerSystem()           = default;
    ~PathControllerTriggerSystem() override = default;

protected:
    ICollisionTriggerComponent* GetTrigger(const OriGine::EntityHandle& _handle) override;
    void ApplyActivate(const OriGine::EntityHandle& _targetHandle) override;
    void ApplyDeactivate(const OriGine::EntityHandle& _targetHandle) override;
};
