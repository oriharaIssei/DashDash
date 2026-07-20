#pragma once

#include "ICameraState.h"

/// <summary>
/// カメラのアイドル状態
/// idleParams へ補間する
/// </summary>
class CameraIdleState : public ICameraState {
public:
    CameraIdleState(OriGine::Scene* _scene, const OriGine::EntityHandle& _cameraEntityHandle, const OriGine::EntityHandle& _playerEntityHandle)
        : ICameraState(_scene, _cameraEntityHandle, _playerEntityHandle) {}
    ~CameraIdleState() override = default;

    void Initialize() override;
    /// <summary>
    /// 現在のオフセット・注視点オフセットをidleParamsへイージング補間する。
    /// </summary>
    void Update() override;
    void Finalize() override;

private:
    const float kLerpTime_ = 0.7f; // idleParamsへの補間にかける時間
    float lerpTimer_       = 0.0f; // 補間の経過時間
};
