#pragma once

#include "component/player/state/PlayerState.h"

/// <summary>
/// プレイヤーの状態が変化したときのイベント
/// </summary>
struct PlayerStateChangedEvent {
    PlayerStateChangedEvent()  = default;
    ~PlayerStateChangedEvent() = default;

    OriGine::EntityHandle playerEntityHandle; // 状態が変化したプレイヤーのエンティティハンドル
    PlayerMoveState previousMoveState; // 変化前の移動状態
    PlayerMoveState currentMoveState; // 変化後の移動状態
};
