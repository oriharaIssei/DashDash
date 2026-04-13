#pragma once
#include "system/ISystem.h"

/// <summary>
/// Initialize時にマウスの状態を指定された状態にするシステム
/// </summary>
class InitializeMouseCondition
    : public OriGine::ISystem {
public:
    InitializeMouseCondition();
    ~InitializeMouseCondition() override = default;

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
