#pragma once

/// parent
#include "component/player/state/IPlayerMoveState.h"

/// <summary>
/// Playerの落下状態 (ジャンプではない)
/// </summary>
class PlayerFallDownState
    : public IPlayerMoveState {
public:
    PlayerFallDownState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle) : IPlayerMoveState(_scene, _playerEntityHandle, PlayerMoveState::FALL_DOWN) {}
    ~PlayerFallDownState() override {};
    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;
    PlayerMoveState TransitionState() const override;

private:
};
