#pragma once
#include "system/ISystem.h"

/// <summary>
/// BackFireの初期化を行うシステム
/// </summary>
class BackFireInitialize
    : public OriGine::ISystem {
public:
    BackFireInitialize();
    ~BackFireInitialize() override;

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
