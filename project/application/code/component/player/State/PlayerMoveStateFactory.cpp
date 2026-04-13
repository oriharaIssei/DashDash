#include "PlayerMoveStateFactory.h"

#include "PlayerDashState.h"
#include "PlayerFallDownState.h"
#include "PlayerIdleState.h"
#include "PlayerJumpState.h"
#include "PlayerOnRailState.h"
#include "PlayerWallJumpState.h"
#include "PlayerWallRunState.h"

std::shared_ptr<IPlayerMoveState> CreatePlayerMoveStateByEnum(PlayerMoveState _state, OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle) {
    switch (_state) {
    case PlayerMoveState::IDLE:
        return std::make_shared<PlayerIdleState>(_scene, _playerEntityHandle);
        break;
    case PlayerMoveState::DASH:
        return std::make_shared<PlayerDashState>(_scene, _playerEntityHandle);
        break;
    case PlayerMoveState::FALL_DOWN:
        return std::make_shared<PlayerFallDownState>(_scene, _playerEntityHandle);
        break;
    case PlayerMoveState::JUMP:
        return std::make_shared<PlayerJumpState>(_scene, _playerEntityHandle);
        break;
    case PlayerMoveState::WALL_RUN:
        return std::make_shared<PlayerWallRunState>(_scene, _playerEntityHandle);
        break;
    case PlayerMoveState::WALL_JUMP:
        return std::make_shared<PlayerWallJumpState>(_scene, _playerEntityHandle);
        break;
    case PlayerMoveState::RUN_ON_RAIL:
        return std::make_shared<PlayerOnRailState>(_scene, _playerEntityHandle);
        break;
    default:
        return nullptr;
        break;
    }
}
