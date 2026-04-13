#pragma once

/// parent
#include "component/player/state/IPlayerMoveState.h"

/// application
#include "component/player/PlayerConfig.h"

/// <summary>
/// Playerのダッシュ状態クラス(基本移動)
/// </summary>
class PlayerDashState
    : public IPlayerMoveState {
public:
    PlayerDashState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle) : IPlayerMoveState(_scene, _playerEntityHandle, PlayerMoveState::DASH) {}
    ~PlayerDashState() override {};

    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;
    PlayerMoveState TransitionState() const override;

private:
    const float kFallDownThresholdTime_ = AppConfig::Player::kDashFallDownThresholdTime; // 落下状態に遷移するまでの閾値時間
    float fallDownTimer_                = 0.0f; // 落下時間計測用
};
