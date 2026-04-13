#pragma once

#include "ICameraState.h"

#include "component/player/PlayerConfig.h"

/// <summary>
/// カメラの追従状態 (Dash / FallDown 共用)
/// ギアレベルに応じて dashParams or idleParams へ補間する
/// </summary>
class CameraFollowState
    : public ICameraState {
public:
    CameraFollowState(OriGine::Scene* _scene, OriGine::EntityHandle _cameraEntityHandle, OriGine::EntityHandle _playerEntityHandle)
        : ICameraState(_scene, _cameraEntityHandle, _playerEntityHandle) {}
    ~CameraFollowState() override = default;

    void Initialize() override;
    void Update() override;
    void Finalize() override;

private:
    const int32_t kThresholdGearLevel_ = AppConfig::Player::kDashCameraOffsetThresholdGearLevel;
    const float kLerpTime_             = AppConfig::Player::kDashCameraOffsetLerpTime;
    float lerpTimer_                   = 0.0f;


};
