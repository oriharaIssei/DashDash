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
    PlayerOnRailState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle);
    ~PlayerOnRailState() override;

    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;

    PlayerMoveState TransitionState() const override;

private:
    bool isOutOfRail_               = false;
    uint32_t currentRailPointIndex_ = 0;
    uint32_t nextRailPointIndex_    = 1;

    OriGine::Vec3f currentDirection_ = OriGine::Vec3f(0.f, 0.f, 0.f);

    float railTotalLength_  = 0.0f;
    float traveledDistance_ = 0.0f;

    float defaultMaxFallDownSpeed_ = 0.f;
    float baseSpeed_               = 0.f;
    float currentSpeed_            = 0.f;
    float rampUpTime_              = 0.0f;
    float rampUpTimer_             = 0.0f;
};
