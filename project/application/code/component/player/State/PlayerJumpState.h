#pragma once

/// parent
#include "component/player/state/IPlayerMoveState.h"

/// <summary>
/// Playerのジャンプ状態クラス
/// </summary>
class PlayerJumpState
    : public IPlayerMoveState {
public:
    PlayerJumpState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle) : IPlayerMoveState(_scene, _playerEntityHandle, PlayerMoveState::JUMP) {}
    ~PlayerJumpState() override {};
    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;
    PlayerMoveState TransitionState() const override;

private:
    float chargePower_      = 0.0f; // ジャンプ力の蓄積量
    float maxChargePower_   = 0.0f; // ジャンプ力の最大値
    float releaseJumpPower_ = 0.0f; // ジャンプボタンを離した時のジャンプパワー
};
