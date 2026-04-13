#pragma once
#include "component/IComponent.h"
#include "PlayerStatus.h"

/// stl
#include <array>

/// component
#include "state/PlayerState.h"

/// math
#include "math/Vector4.h"

/// <summary>
/// Playerのエフェクト制御用パラメータコンポーネント
/// </summary>
class PlayerEffectControlParam
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const PlayerEffectControlParam& _p);
    friend void from_json(const nlohmann::json& j, PlayerEffectControlParam& _p);

public:
    PlayerEffectControlParam();
    ~PlayerEffectControlParam() override;
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

    /// <summary>
    /// ギアレベルに応じたホイールの回転速度を計算する
    /// </summary>
    float CalculateWheelSpinSpeedBySpeed(float _currentSpeed, float _maxSpeed) const;

    /// <summary>
    /// 進行方向と入力方向の角度差に応じたホイールの傾き角度を計算する
    /// </summary>
    /// <param name="_inputV">入力ベクトル</param>
    /// <param name="_direction">進行方向ベクトル</param>
    float CalculateWheelTiltAngle(const OriGine::Vec3f& _inputV, const OriGine::Vec3f& _direction) const;

private:
    std::vector<OriGine::Vec4f> trailColorByGearLevel_ = std::vector<OriGine::Vec4f>(kMaxPlayerGearLevel, OriGine::Vec4f(1.f, 1.f, 1.f, 1.f));
    float rotateOffsetOnWallRun_                       = 0.0f;
    float maxWheelSpinSpeed_                           = 0.001f;
    float wheelTiltAngleRate_                          = 0.0f;
    float preWheelTiltAngle_                           = 0.0f;

    // 障害物衝突時のホイールの傾き角度と傾き速度
    float aheadCollisionCurrentAngle_ = 0.f;
    float aheadCollisionTiltAngle_    = 0.0f;
    float aheadCollisionTiltSpeed_    = 0.0f;

    // 最大傾き角度の加速度
    float wheelTiltAngleMaxAccel_ = 0.0f;
    float maxWheelTiltAngle_      = 0.0f; // ホイールの最大傾き角度

    float maxTiltOnRailRun_   = 0.f;
    float tiltSpeedOnRailRun_ = 0.f;

    // 無敵時点滅エフェクトの基本振幅（開始時の点滅速度）
    float invincibleBlinkBaseAmplitude_ = 8.0f;
    // 無敵時点滅エフェクトの最大振幅（終了間際の点滅速度）
    float invincibleBlinkMaxAmplitude_ = 40.0f;

    // 速度線エフェクトの表示開始速度
    float thresholdSpeedlineParticle_ = 0.f;

    OriGine::EntityHandle tireTrailSplineEntityHandle_ = OriGine::EntityHandle();

public:
    const OriGine::Vec4f& GetTrailColorByGearLevel(int32_t _level) const {
        return trailColorByGearLevel_[_level];
    }
    float GetRotateOffsetOnWallRun() const {
        return rotateOffsetOnWallRun_;
    }
    float GetMaxWheelSpinSpeedRate() const {
        return maxWheelSpinSpeed_;
    };
    float GetWheelTiltAngleRate() const {
        return wheelTiltAngleRate_;
    }
    float GetWheelTiltAngleMaxAccel() const {
        return wheelTiltAngleMaxAccel_;
    }
    float GetMaxWheelTiltAngle() const {
        return maxWheelTiltAngle_;
    }

    float GetMaxTiltOnRailRun() const {
        return maxTiltOnRailRun_;
    }
    float GetTiltSpeedOnRailRun() const {
        return tiltSpeedOnRailRun_;
    }

    float GetInvincibleBlinkBaseAmplitude() const {
        return invincibleBlinkBaseAmplitude_;
    }
    float GetInvincibleBlinkMaxAmplitude() const {
        return invincibleBlinkMaxAmplitude_;
    }

    float GetPreWheelTiltAngle() const {
        return preWheelTiltAngle_;
    }
    void SetPreWheelTiltAngle(float _angle) {
        preWheelTiltAngle_ = _angle;
    }

    float GetThresholdSpeedlineParticle() const {
        return thresholdSpeedlineParticle_;
    }

    OriGine::EntityHandle GetTireTrailSplineEntityHandle() const {
        return tireTrailSplineEntityHandle_;
    }
    void SetTireTrailSplineEntityId(OriGine::EntityHandle _handle) {
        tireTrailSplineEntityHandle_ = _handle;
    }

    float GetAheadCollisionTiltAngle() const {
        return aheadCollisionTiltAngle_;
    }
    void SetAheadCollisionTiltAngle(float _angle) {
        aheadCollisionTiltAngle_ = _angle;
    }

    float GetAheadCollisionTiltSpeed() const {
        return aheadCollisionTiltSpeed_;
    }

    float GetAheadCollisionCurrentAngle() const {
        return aheadCollisionCurrentAngle_;
    }
    void SetAheadCollisionCurrentAngle(float _angle) {
        aheadCollisionCurrentAngle_ = _angle;
    }
};
