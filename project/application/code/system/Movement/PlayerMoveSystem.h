#pragma once
#include "system/ISystem.h"

/// <summary>
/// Playerの移動を制御するシステム
/// </summary>
class PlayerMoveSystem
    : public OriGine::ISystem {
public:
    PlayerMoveSystem() : ISystem(OriGine::SystemCategory::Movement) {}
    ~PlayerMoveSystem() {}

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    // void Update() override;
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
