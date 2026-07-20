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
    CameraFollowState(OriGine::Scene* _scene, const OriGine::EntityHandle& _cameraEntityHandle, const OriGine::EntityHandle& _playerEntityHandle)
        : ICameraState(_scene, _cameraEntityHandle, _playerEntityHandle) {}
    ~CameraFollowState() override = default;

    void Initialize() override;
    /// <summary>
    /// ギアレベルがしきい値以上かどうかでdashParams/idleParamsのどちらへ補間するかを切り替えて更新する。
    /// </summary>
    void Update() override;
    void Finalize() override;

private:
    const int32_t kThresholdGearLevel_ = AppConfig::Player::kDashCameraOffsetThresholdGearLevel; // dashParamsへ切り替えるギアレベルのしきい値
    const float kLerpTime_             = AppConfig::Player::kDashCameraOffsetLerpTime; // 補間にかける時間
    float lerpTimer_                   = 0.0f; // 補間の経過時間


};
