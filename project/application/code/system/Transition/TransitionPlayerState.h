#pragma once
#include "system/ISystem.h"

/// <summary>
/// Player の状態遷移を管理するシステム
/// </summary>
class TransitionPlayerState
    : public OriGine::ISystem {
public:
    TransitionPlayerState() : ISystem(OriGine::SystemCategory::StateTransition) {};
    ~TransitionPlayerState() = default;

    void Initialize() override{}
    void Finalize() override{}

protected:
    /// <summary>
    /// Playerのゴール判定・移動ステート(PlayerMoveState)の生成/遷移・無敵時間や
    /// ペナルティ時間の経過処理・着地カメラシェイクなど、Player状態遷移全般をまとめて更新する。
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
