#pragma once

#include "system/ISystem.h"

/// <summary>
/// 衝突した際に障害物を出現させる
/// </summary>
class ObstacleSpawnEventTriggerSystem
    : public OriGine::ISystem {
public:
    ObstacleSpawnEventTriggerSystem();
    ~ObstacleSpawnEventTriggerSystem() override;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// エンティティの更新処理
    /// </summary>
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
