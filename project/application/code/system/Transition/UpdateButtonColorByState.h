#pragma once

#include "system/ISystem.h"

/// <summary>
/// Buttonの入力状態に応じて色を変更するシステム
/// </summary>
class UpdateButtonColorByState
    : public OriGine::ISystem {
public:
    UpdateButtonColorByState() : ISystem(OriGine::SystemCategory::StateTransition) {}
    ~UpdateButtonColorByState() {}

    virtual void Initialize();
    virtual void Finalize();

protected:
    virtual void UpdateEntity(OriGine::EntityHandle _handle);
};
