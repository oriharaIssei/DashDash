#pragma once

#include "ICameraState.h"

/// <summary>
/// カメラのアイドル状態
/// idleParams へ補間する
/// </summary>
class CameraIdleState : public ICameraState {
public:
    CameraIdleState(OriGine::Scene* _scene, OriGine::EntityHandle _cameraEntityHandle, OriGine::EntityHandle _playerEntityHandle)
        : ICameraState(_scene, _cameraEntityHandle, _playerEntityHandle) {}
    ~CameraIdleState() override = default;

    void Initialize() override;
    void Update() override;
    void Finalize() override;

private:
    const float kLerpTime_ = 0.7f;
    float lerpTimer_       = 0.0f;
};
