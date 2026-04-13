#pragma once

#include "system/ISystem.h"

/// <summary>
/// リスタートボタンが押されたらリスタートを開始する(RestartMessageを飛ばす)システム
/// </summary>
class RestartOnButtonPressSystem
    : public OriGine::ISystem {
public:
    RestartOnButtonPressSystem();
    ~RestartOnButtonPressSystem() override;

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
