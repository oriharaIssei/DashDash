#pragma once
#include "system/ISystem.h"

/// <summary>
/// Player の状態遷移を管理するシステム
/// </summary>
class TransitionPlayerState
    : public OriGine::ISystem {
public:
    TransitionPlayerState() : ISystem(OriGine::SystemCategory::StateTransition) {};
    ~TransitionPlayerState() = default;

    void Initialize() override{}
    void Finalize() override{}

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
