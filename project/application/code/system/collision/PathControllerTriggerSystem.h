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
    ICollisionTriggerComponent* GetTrigger(OriGine::EntityHandle _handle) override;
    void ApplyActivate(OriGine::EntityHandle _targetHandle) override;
    void ApplyDeactivate(OriGine::EntityHandle _targetHandle) override;
};
