#pragma once

#include "system/ISystem.h"

/// <summary>
/// FollowTransformComponentを持つエンティティを対象のTransformに追従させるシステム
/// </summary>
class FollowTransformSystem
    : public OriGine::ISystem {
public:
    FollowTransformSystem();
    ~FollowTransformSystem() override = default;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
