#pragma once
#include "system/ISystem.h"

/// <summary>
/// プレイヤーをスタート位置に移動させるシステム
/// </summary>
class TakePlayerToStartPosition
    : public OriGine::ISystem {
public:
    TakePlayerToStartPosition();
    ~TakePlayerToStartPosition() override;
    virtual void Initialize();
    // virtual void Update();
    // virtual void Edit();
    virtual void Finalize();

protected:
    virtual void UpdateEntity(OriGine::EntityHandle _handle) override;
};
