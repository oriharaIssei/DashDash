#pragma once

#include "system/ISystem.h"

/// <summary>
/// ボタン入力を処理するシステム
/// </summary>
class ButtonInputSystem
    : public OriGine::ISystem {
public:
    ButtonInputSystem() : ISystem(OriGine::SystemCategory::Input) {}
    ~ButtonInputSystem() {}

    virtual void Initialize();
    virtual void Finalize();

protected:
    virtual void UpdateEntity(OriGine::EntityHandle _handle);
};
