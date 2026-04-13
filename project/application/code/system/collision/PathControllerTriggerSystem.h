#pragma once

#include "system/ISystem.h"

/// <summary>
/// PathControllerTrigger を持つ Entity の Collider が衝突 (Enter) したとき、
/// ターゲット Entity の PathController を Activate / Deactivate するシステム。
/// </summary>
class PathControllerTriggerSystem
    : public OriGine::ISystem {
public:
    PathControllerTriggerSystem() : ISystem(OriGine::SystemCategory::Collision) {}
    ~PathControllerTriggerSystem() override = default;

    void Initialize() override {}
    void Finalize() override {}

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
