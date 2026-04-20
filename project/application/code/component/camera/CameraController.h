#pragma once

/// parent
#include "component/IComponent.h"

/// stl
#include <memory>

/// engine
// component
#include "component/transform/Transform.h"

/// application
#include "CameraConfig.h"
#include "CameraStateParams.h"

/// state
#include "state/ICameraState.h"

/// math
#include "math/MyEasing.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

/// <summary>
/// カメラの挙動を制御するためのデータ
/// </summary>
class CameraController
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const CameraController& _cameraController);
    friend void from_json(const nlohmann::json& j, CameraController& _cameraController);

public:
    CameraController() {}
    ~CameraController() override {}

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

    float CalculateFovYBySpeed(float _xzSpeed) const;

    // --- Follow Target ---
    OriGine::EntityHandle GetFollowTargetEntity() const { return followTargetEntity_; }
    void SetFollowTargetEntity(OriGine::EntityHandle _entity) { followTargetEntity_ = _entity; }

    // --- Base Rotation ---
    const OriGine::Quaternion& GetBaseRotate() const { return baseRotate_; }
    void SetBaseRotate(const OriGine::Quaternion& _rotate) { baseRotate_ = _rotate; }

    float GetCurrentRotateZ() const { return currentRotateZ_; }
    void SetCurrentRotateZ(float _z) { currentRotateZ_ = _z; }

    bool GetIsAutoLookAtPlayer() const { return isAutoLookAtPlayer_; }
    void SetIsAutoLookAtPlayer(bool _auto) { isAutoLookAtPlayer_ = _auto; }

    // --- Current Interpolated State ---
    const OriGine::Vec3f& GetCurrentTargetOffset() const { return currentTargetOffset_; }
    void SetCurrentTargetOffset(const OriGine::Vec3f& _offset) { currentTargetOffset_ = _offset; }

    const OriGine::Vec3f& GetCurrentOffset() const { return currentOffset_; }
    void SetCurrentOffset(const OriGine::Vec3f& _offset) { currentOffset_ = _offset; }

    // --- State Params ---
    const CameraStateParams& GetIdleParams() const { return idleParams_; }
    const CameraStateParams& GetDashParams() const { return dashParams_; }
    const CameraStateParams& GetWallRunParams() const { return wallRunParams_; }
    float GetMinTargetOffsetXOnWallRun() const { return minTargetOffsetXOnWallRun_; }

    // --- Follow State Params ---
    float GetTiltDotOnFollow() const { return tiltDotOnFollow_; }
    float GetMaxRollAngleOnFollow() const { return maxRollAngleOnFollow_; }

    // --- Interpolation Targets ---
    const OriGine::Vec3f& GetInterTarget() const { return interTarget_; }
    void SetInterTarget(const OriGine::Vec3f& _target) { interTarget_ = _target; }

    const OriGine::Vec3f& GetInterLookAtTarget() const { return interLookAtTarget_; }
    void SetInterLookAtTarget(const OriGine::Vec3f& _target) { interLookAtTarget_ = _target; }

    const OriGine::Vec2f& GetDestinationAngleXY() const { return destinationAngleXY_; }
    void SetDestinationAngleXY(const OriGine::Vec2f& _angle) { destinationAngleXY_ = _angle; }

    // --- Input Config ---
    float GetRotateSpeedPadStick() const { return rotateSpeedPadStick_; }
    float GetRotateSpeedMouse() const { return rotateSpeedMouse_; }
    float GetRotateSensitivity() const { return rotateSensitivity_; }

    const OriGine::Vec3f& GetInterTargetInterpolation() const { return interTargetInterpolation_; }
    const OriGine::Vec3f& GetInterLookAtTargetInterpolation() const { return interLookAtTargetInterpolation_; }

    // --- Rotation Limits ---
    float GetMaxRotateX() const { return maxRotateX_; }
    float GetMinRotateX() const { return minRotateX_; }

    // --- FOV ---
    float GetFovYInterpolate() const { return fovYInterpolate_; }

    // --- Camera State ---
    const std::shared_ptr<ICameraState>& GetCameraState() const { return cameraState_; }
    void SetCameraState(std::shared_ptr<ICameraState> _state) { cameraState_ = std::move(_state); }

    CameraMoveState GetCurrentCameraStateType() const { return currentCameraStateType_; }
    void SetCurrentCameraStateType(CameraMoveState _type) { currentCameraStateType_ = _type; }

private:
    OriGine::EntityHandle followTargetEntity_ = OriGine::EntityHandle();

    OriGine::Quaternion baseRotate_ = OriGine::Quaternion::Identity();
    float currentRotateZ_           = 0.0f;

    bool isAutoLookAtPlayer_ = true;

    OriGine::Vec3f currentTargetOffset_ = {0.0f, 0.0f, 0.0f};
    OriGine::Vec3f currentOffset_       = {0.0f, 0.0f, 0.0f};

    CameraStateParams idleParams_{
        AppConfig::Camera::kFirstTargetOffset,
        AppConfig::Camera::kFirstOffset,
        AppConfig::Camera::kDefaultInterTargetInterpolation,
        AppConfig::Camera::kDefaultInterTargetInterpolation};
    CameraStateParams dashParams_{
        AppConfig::Camera::kTargetOffsetOnDash,
        AppConfig::Camera::kOffsetOnDash,
        AppConfig::Camera::kDefaultInterTargetInterpolation,
        AppConfig::Camera::kDefaultInterTargetInterpolation};
    CameraStateParams wallRunParams_{
        AppConfig::Camera::kTargetOffsetOnDash,
        AppConfig::Camera::kOffsetOnDash,
        AppConfig::Camera::kDefaultInterTargetInterpolation,
        AppConfig::Camera::kDefaultInterTargetInterpolation};

    float minTargetOffsetXOnWallRun_ = 0.0f;

    float tiltDotOnFollow_      = 0.f;
    float maxRollAngleOnFollow_ = 0.f;

    OriGine::Vec3f interTarget_                    = {0.0f, 0.0f, 0.0f};
    OriGine::Vec3f interLookAtTarget_              = {0.0f, 0.0f, 0.0f};
    OriGine::Vec2f destinationAngleXY_             = AppConfig::Camera::kDefaultDestinationAngle;
    float rotateSpeedPadStick_                     = 0.0f;
    float rotateSpeedMouse_                        = 0.0f;
    float rotateSensitivity_                       = AppConfig::Camera::kDefaultRotateSensitivity;
    OriGine::Vec3f interTargetInterpolation_       = AppConfig::Camera::kDefaultInterTargetInterpolation;
    OriGine::Vec3f interLookAtTargetInterpolation_ = AppConfig::Camera::kDefaultInterTargetInterpolation;

    float maxRotateX_ = 0.0f;
    float minRotateX_ = 0.0f;

    float fovYInterpolate_         = AppConfig::Camera::kDefaultFovYInterpolate;
    float fovMin_                  = AppConfig::Camera::kDefaultFovMin;
    float fovMax_                  = AppConfig::Camera::kDefaultFovMax;
    float fovMinSpeed_             = AppConfig::Camera::kDefaultFovMinSpeed;
    float fovMaxSpeed_             = AppConfig::Camera::kDefaultFovMaxSpeed;
    OriGine::EaseType fovEaseType_ = OriGine::EaseType::Linear;
    float fixForForwardSpeed_      = AppConfig::Camera::kFixForForwardSpeed;

    std::shared_ptr<ICameraState> cameraState_ = nullptr;
    CameraMoveState currentCameraStateType_    = CameraMoveState::IDLE;
};
