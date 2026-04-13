#pragma once
/// parent
#include "component/IComponent.h"

/// stl
#include <vector>

/// engine
#include "scene/Scene.h"
// lib
#include "input/InputManager.h"

/// component
#include "component/player/state/PlayerState.h"

/// math
#include "math/Vector2.h"
#include "math/Vector3.h"

/// <summary>
/// Playerの移動状態を表すインターフェース
/// </summary>
class IPlayerMoveState {
public:
    IPlayerMoveState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle, PlayerMoveState _state);
    virtual ~IPlayerMoveState();

    virtual void Initialize()             = 0;
    virtual void Update(float _deltaTime) = 0;
    virtual void Finalize()               = 0;

    /// <summary>
    /// Playerの状態遷移を行う(遷移条件,遷移先は派生クラスで実装)
    /// </summary>
    /// <returns></returns>
    virtual PlayerMoveState TransitionState() const = 0;

protected:
    OriGine::Scene* scene_           = nullptr; // シーンへのポインタ
    OriGine::EntityHandle playerEntityHandle_ = OriGine::EntityHandle(); // プレイヤーのエンティティID
private:
    // このクラスが表す移動状態
    PlayerMoveState state_ = PlayerMoveState::IDLE;

public:
    const PlayerMoveState& GetState() const {
        return state_;
    }
    void SetState(const PlayerMoveState& _state) {
        state_ = _state;
    }
};
