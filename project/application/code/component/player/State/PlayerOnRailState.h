#pragma once

#include "IPlayerMoveState.h"

/// math
#include <Vector3.h>

/// <summary>
/// プレイヤーがレールに乗っている状態のクラス
/// </summary>
class PlayerOnRailState
    : public IPlayerMoveState {
public:
    PlayerOnRailState(OriGine::Scene* _scene, const OriGine::EntityHandle& _playerEntityHandle);
    ~PlayerOnRailState() override;

    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;
    PlayerMoveState TransitionState() const override;
    JumpInputResponse OnJumpTriggered() const override { return {.railJump = true}; }

private:
    bool isOutOfRail_               = false; // レールから外れたかどうか
    uint32_t currentRailPointIndex_ = 0; // 現在いる区間の始点となるレールポイントのインデックス
    uint32_t nextRailPointIndex_    = 1; // 現在いる区間の終点となるレールポイントのインデックス

    OriGine::Vec3f currentDirection_ = OriGine::Vec3f(0.f, 0.f, 0.f); // 現在の区間の進行方向ベクトル(未正規化)

    float railTotalLength_  = 0.0f; // レール全体の長さ
    float traveledDistance_ = 0.0f; // レール上で移動した距離

    float defaultMaxFallDownSpeed_ = 0.f; // レールに乗る前の最大落下速度(状態終了時に復元する)
    float baseSpeed_               = 0.f; // レールに乗った時点でのプレイヤーの基本速度
    float currentSpeed_            = 0.f; // 現在のレール上移動速度
    float rampUpTime_              = 0.0f; // 速度がレール速度倍率まで上がりきるまでの時間
    float rampUpTimer_             = 0.0f; // 速度が上がり始めてからの経過時間
};
