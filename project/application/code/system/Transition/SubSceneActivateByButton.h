#pragma once

#include "system/ISystem.h"

/// <summary>
/// ボタン入力でサブシーンを有効化するシステム
/// </summary>
class SubSceneActivateByButton
    : public OriGine::ISystem {
public:
    SubSceneActivateByButton();
    ~SubSceneActivateByButton() override;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
