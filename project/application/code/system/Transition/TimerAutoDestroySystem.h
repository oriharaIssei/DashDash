#pragma once
#include "system/ISystem.h"

/// <summary>
/// タイマー自動破棄システム
/// TimerComponentのcurrentTime_が0以下になったらEntityを削除する
/// </summary>
class TimerAutoDestroySystem
    : public OriGine::ISystem {
public:
    TimerAutoDestroySystem();
    ~TimerAutoDestroySystem() override;
    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
