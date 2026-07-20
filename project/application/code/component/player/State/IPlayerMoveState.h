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
/// ジャンプ入力に対する応答
/// </summary>
struct JumpInputResponse {
    bool wallJump = false; // 壁ジャンプとして扱うか
    bool railJump = false; // レールジャンプとして扱うか
};

/// <summary>
/// Playerの移動状態を表すインターフェース
/// </summary>
class IPlayerMoveState {
public:
    IPlayerMoveState(OriGine::Scene* _scene, const OriGine::EntityHandle& _playerEntityHandle, PlayerMoveState _state);
    virtual ~IPlayerMoveState();

    /// <summary>
    /// 状態開始時の初期化処理(派生クラスで実装)
    /// </summary>
    virtual void Initialize() = 0;
    /// <summary>
    /// 毎フレームの更新処理(派生クラスで実装)
    /// </summary>
    virtual void Update(float _deltaTime) = 0;
    /// <summary>
    /// 状態終了時の終了処理(派生クラスで実装)
    /// </summary>
    virtual void Finalize() = 0;

    /// <summary>
    /// Playerの状態遷移を行う(遷移条件,遷移先は派生クラスで実装)
    /// </summary>
    virtual PlayerMoveState TransitionState() const = 0;

    /// <summary>
    /// ジャンプボタン長押しを許可するか (デフォルト: false)
    /// </summary>
    virtual bool CanHoldJump() const { return false; }

    /// <summary>
    /// ジャンプボタンが押された瞬間の応答 (デフォルト: 通常ジャンプ)
    /// </summary>
    virtual JumpInputResponse OnJumpTriggered() const { return {}; }

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
