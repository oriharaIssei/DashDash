#pragma once
#include "system/ISystem.h"

/// <summary>
/// 3D UI更新処理を行うシステム
/// </summary>
class Ui3dUpdateSystem
    : public OriGine::ISystem {
public:
    Ui3dUpdateSystem();
    ~Ui3dUpdateSystem();

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
