#pragma once
#include "system/ISystem.h"

/// <summary>
/// ステージをクリアしたときのタイムを取得するシステム
/// </summary>
class GetClearTime
    : public OriGine::ISystem {
public:
    GetClearTime();
    ~GetClearTime() override;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
