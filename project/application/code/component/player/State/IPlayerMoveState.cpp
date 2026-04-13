#include "IPlayerMoveState.h"

IPlayerMoveState::IPlayerMoveState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle, PlayerMoveState _state)
    : scene_(_scene), playerEntityHandle_(_playerEntityHandle), state_(_state) {}
IPlayerMoveState::~IPlayerMoveState() {}
