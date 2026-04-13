#pragma once
#include "system/ISystem.h"

/// <summary>
/// Playerの衝突反応システム
/// </summary>
class PlayerOnCollision
    : public OriGine::ISystem {
public:
    PlayerOnCollision(int32_t _priority = 0)
        : ISystem(OriGine::SystemCategory::Collision, _priority) {};
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    // void Update() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// エンティティの更新処理
    /// </summary>
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
