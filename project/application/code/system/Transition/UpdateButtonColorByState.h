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
    /// <summary>
    /// Buttonの状態(Released/Pressed/Hovered/Normal)に応じた目標色へ、
    /// SpriteRendererの色を線形補間しながら近づける。
    /// </summary>
    virtual void UpdateEntity(const OriGine::EntityHandle& _handle);
};
