#pragma once

#include "system/ISystem.h"

/// <summary>
/// ButtonGroupによるボタン操作システム
/// </summary>
class ButtonGroupSystem
    : public OriGine::ISystem {
public:
    ButtonGroupSystem();
    ~ButtonGroupSystem() override = default;

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
