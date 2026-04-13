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
/// 速度補正コンポーネント
/// </summary>
struct SpeedModifiers
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

    /// <summary>
    /// 加算効果の開始
    /// </summary>
    /// <param name="_target"></param>
    /// <param name="_lerpDuration"></param>
    /// <param name="_effectDuration"></param>
    /// <param name="_easeType"></param>
    /// <param name="_beforeSpeed"></param>
    void StartAdditiveEffect(float _target, float _lerpDuration, float _effectDuration, OriGine::EaseType _easeType, float _beforeSpeed, float _restoreSpeed, float _fadeOutDuration = 0.f, OriGine::EaseType _fadeOutEaseType = OriGine::EaseType::Linear);
    /// <summary>
    /// 乗算効果の開始
    /// </summary>
    /// <param name="_target"></param>
    /// <param name="_lerpDuration"></param>
    /// <param name="_effectDuration"></param>
    /// <param name="_easeType"></param>
    /// <param name="_beforeSpeed"></param>
    void StartMultiplierEffect(float _target, float _lerpDuration, float _effectDuration, OriGine::EaseType _easeType, float _beforeSpeed, float _restoreSpeed, float _fadeOutDuration = 0.f, OriGine::EaseType _fadeOutEaseType = OriGine::EaseType::Linear);

    bool IsAdditiveEffectActive() const {
        return additiveTimer < additiveDuration || additiveFadeOutTimer < additiveFadeOutDuration;
    }
    bool IsMultiplierEffectActive() const {
        return multiplierTimer < multiplierDuration || multiplierFadeOutTimer < multiplierFadeOutDuration;
    }

public:
    bool isAutoDestroyed = true; // 効果時間終了後に自動で削除されるかどうか

    OriGine::ComponentHandle targetRigidbodyHandle{};

    /// <summary>
    /// トリガーモード
    /// </summary>
    enum class TriggerMode : int {
        OnEnter        = 0, ///< 衝突開始時に1回生成、タイマーで管理
        WhileColliding = 1, ///< 衝突中ずっと適用、Exit時にFadeOut開始
    };
    TriggerMode triggerMode = TriggerMode::OnEnter;

    /// <summary>
    /// 軸の空間
    /// </summary>
    enum class AxesSpace : int {
        World    = 0, ///< ワールド軸 (X, Y, Z)
        Velocity = 1, ///< 速度相対軸 (Front, Side, Up)
    };
    AxesSpace axesSpace = AxesSpace::World;

    // Additive
    float additiveTarget                      = 0.f;
    float additiveFadeInTimer                 = 0.0f;
    float additiveFadeInDuration                = 0.0f;
    float additiveDuration                    = 0.0f;
    float additiveTimer                       = 0.0f;
    float additiveFadeOutDuration             = 0.0f;
    float additiveFadeOutTimer                = 0.0f;
    OriGine::EaseType additiveLerpEaseType    = OriGine::EaseType::Linear;
    OriGine::EaseType additiveFadeOutEaseType = OriGine::EaseType::Linear;
    std::array<bool, 3> additiveAxes          = {true, true, true}; // 適用する軸 (X, Y, Z)

    // Multiplier
    float multiplierTarget                      = 1.f;
    float multiplierFadeInTimer                 = 0.0f;
    float multiplierFadeInDuration                = 0.0f;
    float multiplierDuration                    = 0.0f;
    float multiplierTimer                       = 0.0f;
    float multiplierFadeOutDuration             = 0.0f;
    float multiplierFadeOutTimer                = 0.0f;
    OriGine::EaseType multiplierLerpEaseType    = OriGine::EaseType::Linear;
    OriGine::EaseType multiplierFadeOutEaseType = OriGine::EaseType::Linear;
    std::array<bool, 3> multiplierAxes          = {true, true, true}; // 適用する軸 (X, Y, Z)

    float beforeSpeed  = 0.0f;
    float restoreSpeed = 0.0f;
};
