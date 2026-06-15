#pragma once
#include "system/ISystem.h"

/// <summary>
/// Buttonによるアプリケーション終了システム
/// </summary>
class ExitApplicationByButton
    : public OriGine::ISystem {
public:
    ExitApplicationByButton();
    ~ExitApplicationByButton() override;

    void Initialize() override {}
    void Finalize() override {}

private:
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
