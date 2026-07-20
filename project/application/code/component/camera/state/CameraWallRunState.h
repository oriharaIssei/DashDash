#pragma once

#include "ICameraState.h"

#include "math/Vector3.h"

/// <summary>
/// カメラのWallRun状態
/// wallRunParams へ補間 + 入力に応じた targetOffset.X 調整
/// </summary>
class CameraWallRunState : public ICameraState {
public:
    CameraWallRunState(OriGine::Scene* _scene, const OriGine::EntityHandle& _cameraEntityHandle, const OriGine::EntityHandle& _playerEntityHandle)
        : ICameraState(_scene, _cameraEntityHandle, _playerEntityHandle) {}
    ~CameraWallRunState() override = default;

    void Initialize() override;
    void Update() override;
    void Finalize() override;

private:
    // 壁の左右に応じて反転済みの、補間先targetOffset/minTargetOffsetX/offset
    OriGine::Vec3f targetOffsetOnWallRun_     = {0.0f, 0.0f, 0.0f};
    float minTargetOffsetXOnWallRun_          = 0.0f;
    OriGine::Vec3f offsetOnWallRun_           = {0.0f, 0.0f, 0.0f};

    bool isRightWall_ = false; // 右壁を走っているかどうか

    const float kLerpTime_ = 1.8f; // wallRunParamsへの補間にかける時間
    float lerpTimer_       = 0.0f; // 補間の経過時間
};
