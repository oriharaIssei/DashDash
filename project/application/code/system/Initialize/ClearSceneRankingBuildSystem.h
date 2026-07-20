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
    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// プレイした最新ステージの進行状況からクリアタイムランキングを取得し、Timerコンポーネントへ反映する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
