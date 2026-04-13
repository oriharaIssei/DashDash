#pragma once
#include "system/ISystem.h"

/// <summary>
/// クリアシーンでランキング表示をビルドするシステム
/// </summary>
class ClearSceneRankingBuildSystem
    : public OriGine::ISystem {
public:
    ClearSceneRankingBuildSystem();
    ~ClearSceneRankingBuildSystem() override;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
