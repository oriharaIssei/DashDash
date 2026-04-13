#pragma once

#include "IPlayerMoveState.h"

#include "component/physics/Rigidbody.h"
#include "component/transform/Transform.h"

class StageWall;

/// <summary>
/// Playerの壁ジャンプ状態
/// </summary>
class PlayerWallJumpState
    : public IPlayerMoveState {
public:
    PlayerWallJumpState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle) : IPlayerMoveState(_scene, _playerEntityHandle, PlayerMoveState::WALL_JUMP) {}
    ~PlayerWallJumpState() override {};

    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;

    PlayerMoveState TransitionState() const override;

protected:
    OriGine::Vec3f velo_ = OriGine::Vec3f(0.0f, 0.0f, 0.0f); // 壁ジャンプの速度

    const float kForceJumpTime_ = 0.14f;
    float forceJumpTimer_       = 0.0f;
};
