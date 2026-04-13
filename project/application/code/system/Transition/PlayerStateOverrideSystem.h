#pragma once
#include "system/ISystem.h"

/// <summary>
/// プレイヤーの状態が指定された状態になるまで 時間を止めるシステム
/// </summary>
class PlayerStateOverrideSystem
    : public OriGine::ISystem {
public:
    PlayerStateOverrideSystem();
    ~PlayerStateOverrideSystem() override;
    void Initialize() override;
    void Finalize() override;

protected:
    void Update() override;
    void UpdateEntity(OriGine::EntityHandle _handle) override;

private:
    bool isTimeScaled_ = false;
};
