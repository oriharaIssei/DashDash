#pragma once

#include "component/IComponent.h"

/// stl
#include <array>

/// ECS
// component
#include "component/ComponentHandle.h"

/// math
#include "math/Vector3.h"
#include <math/MyEasing.h>

namespace OriGine {
class Rigidbody;
}

/// <summary>
/// トリガーモード
/// </summary>
enum class SpeedModifierTriggerMode : int {
    OnEnter        = 0, ///< 衝突開始時に1回生成、タイマーで管理
    WhileColliding = 1, ///< 衝突中ずっと適用、Exit時にFadeOut開始
};

/// <summary>
/// 軸の空間
/// </summary>
enum class SpeedModifierAxesSpace : int {
    World    = 0, ///< ワールド軸 (X, Y, Z)
    Velocity = 1, ///< 速度相対軸 (Front, Side, Up)
};

/// <summary>
/// 速度補正コンポーネント
/// </summary>
class SpeedModifiers
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const SpeedModifiers& _c);
    friend void from_json(const nlohmann::json& _j, SpeedModifiers& _c);

public:
    SpeedModifiers();
    ~SpeedModifiers();

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

    void Reset();

    void StartAdditiveEffect(float _target, float _lerpDuration, float _effectDuration, OriGine::EaseType _easeType, float _beforeSpeed, float _restoreSpeed, float _fadeOutDuration = 0.f, OriGine::EaseType _fadeOutEaseType = OriGine::EaseType::Linear);
    void StartMultiplierEffect(float _target, float _lerpDuration, float _effectDuration, OriGine::EaseType _easeType, float _beforeSpeed, float _restoreSpeed, float _fadeOutDuration = 0.f, OriGine::EaseType _fadeOutEaseType = OriGine::EaseType::Linear);

    bool IsAdditiveEffectActive() const {
        return additiveTimer_ < additiveDuration_ || additiveFadeOutTimer_ < additiveFadeOutDuration_;
    }
    bool IsMultiplierEffectActive() const {
        return multiplierTimer_ < multiplierDuration_ || multiplierFadeOutTimer_ < multiplierFadeOutDuration_;
    }

    // --- General ---
    bool GetIsAutoDestroyed() const { return isAutoDestroyed_; }
    void SetIsAutoDestroyed(bool _v) { isAutoDestroyed_ = _v; }

    const OriGine::ComponentHandle& GetTargetRigidbodyHandle() const { return targetRigidbodyHandle_; }
    void SetTargetRigidbodyHandle(const OriGine::ComponentHandle& _h) { targetRigidbodyHandle_ = _h; }

    SpeedModifierTriggerMode GetTriggerMode() const { return triggerMode_; }
    void SetTriggerMode(SpeedModifierTriggerMode _m) { triggerMode_ = _m; }

    SpeedModifierAxesSpace GetAxesSpace() const { return axesSpace_; }
    void SetAxesSpace(SpeedModifierAxesSpace _s) { axesSpace_ = _s; }

    // --- Additive ---
    float GetAdditiveTarget() const { return additiveTarget_; }
    float GetAdditiveFadeInTimer() const { return additiveFadeInTimer_; }
    void SetAdditiveFadeInTimer(float _v) { additiveFadeInTimer_ = _v; }
    float GetAdditiveFadeInDuration() const { return additiveFadeInDuration_; }
    float GetAdditiveDuration() const { return additiveDuration_; }
    void SetAdditiveDuration(float _v) { additiveDuration_ = _v; }
    float GetAdditiveTimer() const { return additiveTimer_; }
    void SetAdditiveTimer(float _v) { additiveTimer_ = _v; }
    float GetAdditiveFadeOutDuration() const { return additiveFadeOutDuration_; }
    float GetAdditiveFadeOutTimer() const { return additiveFadeOutTimer_; }
    void SetAdditiveFadeOutTimer(float _v) { additiveFadeOutTimer_ = _v; }
    OriGine::EaseType GetAdditiveLerpEaseType() const { return additiveLerpEaseType_; }
    OriGine::EaseType GetAdditiveFadeOutEaseType() const { return additiveFadeOutEaseType_; }
    const std::array<bool, 3>& GetAdditiveAxes() const { return additiveAxes_; }

    // --- Multiplier ---
    float GetMultiplierTarget() const { return multiplierTarget_; }
    float GetMultiplierFadeInTimer() const { return multiplierFadeInTimer_; }
    void SetMultiplierFadeInTimer(float _v) { multiplierFadeInTimer_ = _v; }
    float GetMultiplierFadeInDuration() const { return multiplierFadeInDuration_; }
    float GetMultiplierDuration() const { return multiplierDuration_; }
    void SetMultiplierDuration(float _v) { multiplierDuration_ = _v; }
    float GetMultiplierTimer() const { return multiplierTimer_; }
    void SetMultiplierTimer(float _v) { multiplierTimer_ = _v; }
    float GetMultiplierFadeOutDuration() const { return multiplierFadeOutDuration_; }
    float GetMultiplierFadeOutTimer() const { return multiplierFadeOutTimer_; }
    void SetMultiplierFadeOutTimer(float _v) { multiplierFadeOutTimer_ = _v; }
    OriGine::EaseType GetMultiplierLerpEaseType() const { return multiplierLerpEaseType_; }
    OriGine::EaseType GetMultiplierFadeOutEaseType() const { return multiplierFadeOutEaseType_; }
    const std::array<bool, 3>& GetMultiplierAxes() const { return multiplierAxes_; }

    // --- Speed ---
    float GetBeforeSpeed() const { return beforeSpeed_; }
    void SetBeforeSpeed(float _v) { beforeSpeed_ = _v; }
    float GetRestoreSpeed() const { return restoreSpeed_; }
    void SetRestoreSpeed(float _v) { restoreSpeed_ = _v; }

private:
    bool isAutoDestroyed_ = true;

    OriGine::ComponentHandle targetRigidbodyHandle_{};

    SpeedModifierTriggerMode triggerMode_ = SpeedModifierTriggerMode::OnEnter;
    SpeedModifierAxesSpace axesSpace_     = SpeedModifierAxesSpace::World;

    // Additive
    float additiveTarget_                      = 0.f;
    float additiveFadeInTimer_                 = 0.0f;
    float additiveFadeInDuration_              = 0.0f;
    float additiveDuration_                    = 0.0f;
    float additiveTimer_                       = 0.0f;
    float additiveFadeOutDuration_             = 0.0f;
    float additiveFadeOutTimer_                = 0.0f;
    OriGine::EaseType additiveLerpEaseType_    = OriGine::EaseType::Linear;
    OriGine::EaseType additiveFadeOutEaseType_ = OriGine::EaseType::Linear;
    std::array<bool, 3> additiveAxes_          = {true, true, true};

    // Multiplier
    float multiplierTarget_                      = 1.f;
    float multiplierFadeInTimer_                 = 0.0f;
    float multiplierFadeInDuration_              = 0.0f;
    float multiplierDuration_                    = 0.0f;
    float multiplierTimer_                       = 0.0f;
    float multiplierFadeOutDuration_             = 0.0f;
    float multiplierFadeOutTimer_                = 0.0f;
    OriGine::EaseType multiplierLerpEaseType_    = OriGine::EaseType::Linear;
    OriGine::EaseType multiplierFadeOutEaseType_ = OriGine::EaseType::Linear;
    std::array<bool, 3> multiplierAxes_          = {true, true, true};

    float beforeSpeed_  = 0.0f;
    float restoreSpeed_ = 0.0f;
};
