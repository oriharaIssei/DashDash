#pragma once

/// stl
#include <concepts>
#include <memory>

/// ECS
// component
#include "IPlayerMoveState.h"
#include "PlayerState.h"

/// <summary>
/// 指定された列挙型に基づいてプレイヤーの移動状態を生成するファクトリ関数
/// </summary>
std::shared_ptr<IPlayerMoveState> CreatePlayerMoveStateByEnum(PlayerMoveState _state, OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle);
