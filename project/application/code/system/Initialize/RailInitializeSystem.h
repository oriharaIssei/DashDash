#pragma once

#include "system/ISystem.h"

/// <summary>
/// Rail 初期化システム
/// </summary>
class RailInitializeSystem
    : public OriGine::ISystem {
public:
    RailInitializeSystem();
    ~RailInitializeSystem();

    void Initialize() override;
    void Finalize() override;

    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
