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
struct CameraController
    : public OriGine::IComponent {
    /// <summary>
    /// JSON 変換用
    /// </summary>
    friend void to_json(nlohmann::json& j, const CameraController& _cameraController);
    /// <summary>
    /// JSON 復元用
    /// </summary>
    friend void from_json(const nlohmann::json& j, CameraController& _cameraController);

public:
    CameraController() {}
    ~CameraController() override {}

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

    /// <summary>
    /// XZ平面の速度からFovYを計算する
    /// </summary>
    /// <param name="_xzSpeed">XZ平面の速度</param>
    /// <returns></returns>
    float CalculateFovYBySpeed(float _xzSpeed) const;

public:
    OriGine::EntityHandle followTargetEntity = OriGine::EntityHandle();

    // 演出用の回転と分けるための基準回転
    OriGine::Quaternion baseRotate = OriGine::Quaternion::Identity();
    float currentRotateZ           = 0.0f; // カメラのZ回転（ロール） 現在値

    bool isAutoLookAtPlayer = true; // 自動でプレイヤーを注視するかどうか

    /// <summary>
    /// 現在のカメラステートパラメータ (補間後)
    /// </summary>
    OriGine::Vec3f currentTargetOffset = {0.0f, 0.0f, 0.0f};
    OriGine::Vec3f currentOffset       = {0.0f, 0.0f, 0.0f};

    /// <summary>
    /// ステートごとのパラメータ
    /// </summary>
    CameraStateParams idleParams{
        AppConfig::Camera::kFirstTargetOffset,
        AppConfig::Camera::kFirstOffset,
        AppConfig::Camera::kDefaultInterTargetInterpolation,
        AppConfig::Camera::kDefaultInterTargetInterpolation};
    CameraStateParams dashParams{
        AppConfig::Camera::kTargetOffsetOnDash,
        AppConfig::Camera::kOffsetOnDash,
        AppConfig::Camera::kDefaultInterTargetInterpolation,
        AppConfig::Camera::kDefaultInterTargetInterpolation};
    CameraStateParams wallRunParams{
        AppConfig::Camera::kTargetOffsetOnDash,
        AppConfig::Camera::kOffsetOnDash,
        AppConfig::Camera::kDefaultInterTargetInterpolation,
        AppConfig::Camera::kDefaultInterTargetInterpolation};

    float minTargetOffsetXOnWallRun = 0.0f;

    // State : Follow で使用するパラメータ
    float tiltDotOnFollow      = 0.f;
    float maxRollAngleOnFollow = 0.f;

    OriGine::Vec3f interTarget                    = {0.0f, 0.0f, 0.0f};
    OriGine::Vec3f interLookAtTarget              = {0.0f, 0.0f, 0.0f}; // XY注視用補間値（カメラ位置は動かさず回転のみ）
    OriGine::Vec2f destinationAngleXY             = AppConfig::Camera::kDefaultDestinationAngle;
    float rotateSpeedPadStick                     = 0.0f;
    float rotateSpeedMouse                        = 0.0f;
    float rotateSensitivity                       = AppConfig::Camera::kDefaultRotateSensitivity;
    OriGine::Vec3f interTargetInterpolation       = AppConfig::Camera::kDefaultInterTargetInterpolation;
    OriGine::Vec3f interLookAtTargetInterpolation = AppConfig::Camera::kDefaultInterTargetInterpolation;

    float maxRotateX = 0.0f;
    float minRotateX = 0.0f;

    float fovYInterpolate         = AppConfig::Camera::kDefaultFovYInterpolate;
    float fovMin                  = AppConfig::Camera::kDefaultFovMin; // FovYの最小値
    float fovMax                  = AppConfig::Camera::kDefaultFovMax; // FovYの最大値
    float fovMinSpeed             = AppConfig::Camera::kDefaultFovMinSpeed; // FovY最小になる速度
    float fovMaxSpeed             = AppConfig::Camera::kDefaultFovMaxSpeed; // FovY最大になる速度
    OriGine::EaseType fovEaseType = OriGine::EaseType::Linear; // 速度→FovY のイージング
    float fixForForwardSpeed      = AppConfig::Camera::kFixForForwardSpeed; // 正面方向への 補正速度

    /// <summary>
    /// カメラステート
    /// </summary>
    std::shared_ptr<ICameraState> cameraState_ = nullptr;
    CameraMoveState currentCameraStateType_    = CameraMoveState::IDLE;
};
