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
    /// <summary>
    /// ButtonGroup内の選択状態・決定入力を更新する。
    /// キーボード/ゲームパッド/マウス等、外部システムによる直接選択と、
    /// 選択ボタンの上下(左右)入力・決定入力の両方を扱う。
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
