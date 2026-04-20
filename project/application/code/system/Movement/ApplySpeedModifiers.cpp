#include "ApplySpeedModifiers.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "component/SpeedModifiers.h"

/// math
#include "math/Interpolation.h"
#include "MathEnv.h"
#include <MyEasing.h>
#include <Vector3.h>

using namespace OriGine;

ApplySpeedModifiers::ApplySpeedModifiers() : ISystem(SystemCategory::Movement) {}
ApplySpeedModifiers::~ApplySpeedModifiers() {}

void ApplySpeedModifiers::Initialize() {}
void ApplySpeedModifiers::Finalize() {}

void ApplySpeedModifiers::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& speedModifiers = GetComponents<SpeedModifiers>(_handle);

    float deltaTime    = 0.f;
    float defDeltaTime = Engine::GetInstance()->GetDeltaTimer()->GetDeltaTime();

    for (auto& speedModifier : speedModifiers) {
        // 対象のRigidbodyコンポーネントを取得
        auto rigidbodyComp = GetComponent<Rigidbody>(speedModifier.GetTargetRigidbodyHandle());
        if (!rigidbodyComp) {
            continue;
        }

        if (rigidbodyComp->IsUsingLocalDeltaTime()) {
            deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime(rigidbodyComp->GetLocalDeltaTimeName());
        } else {
            deltaTime = defDeltaTime;
        }

        float t               = 0.f;
        float additiveSpeed   = 0.f;
        float multiplierSpeed = 1.f;

        speedModifier.SetAdditiveTimer(speedModifier.GetAdditiveTimer() + deltaTime);
        speedModifier.SetAdditiveFadeInTimer(speedModifier.GetAdditiveFadeInTimer() + deltaTime);
        if (speedModifier.GetAdditiveFadeInTimer() <= speedModifier.GetAdditiveFadeInDuration()) {
            // FadeIn フェーズ
            t             = (std::min)(speedModifier.GetAdditiveFadeInTimer() / speedModifier.GetAdditiveFadeInDuration(), 1.0f);
            additiveSpeed = std::lerp(0.f, speedModifier.GetAdditiveTarget(), EasingFunctions[static_cast<int>(speedModifier.GetAdditiveLerpEaseType())](t));
        } else if (speedModifier.GetAdditiveTimer() < speedModifier.GetAdditiveDuration()) {
            // Hold フェーズ
            additiveSpeed = speedModifier.GetAdditiveTarget();
        } else {
            // FadeOut フェーズ
            if (speedModifier.GetAdditiveFadeOutDuration() > 0.f) {
                speedModifier.SetAdditiveFadeOutTimer((std::min)(speedModifier.GetAdditiveFadeOutTimer() + deltaTime, speedModifier.GetAdditiveFadeOutDuration()));
                t             = speedModifier.GetAdditiveFadeOutTimer() / speedModifier.GetAdditiveFadeOutDuration();
                additiveSpeed = std::lerp(speedModifier.GetAdditiveTarget(), 0.f, EasingFunctions[static_cast<int>(speedModifier.GetAdditiveFadeOutEaseType())](t));
            } else {
                additiveSpeed = LerpByDeltaTime(speedModifier.GetAdditiveTarget(), 0.f, deltaTime, speedModifier.GetRestoreSpeed());
            }
        }

        speedModifier.SetMultiplierTimer(speedModifier.GetMultiplierTimer() + deltaTime);
        speedModifier.SetMultiplierFadeInTimer(speedModifier.GetMultiplierFadeInTimer() + deltaTime);
        if (speedModifier.GetMultiplierFadeInTimer() <= speedModifier.GetMultiplierFadeInDuration()) {
            // FadeIn フェーズ
            t               = (std::min)(speedModifier.GetMultiplierFadeInTimer() / speedModifier.GetMultiplierFadeInDuration(), 1.0f);
            multiplierSpeed = std::lerp(1.f, speedModifier.GetMultiplierTarget(), EasingFunctions[static_cast<int>(speedModifier.GetMultiplierLerpEaseType())](t));
        } else if (speedModifier.GetMultiplierTimer() < speedModifier.GetMultiplierDuration()) {
            // Hold フェーズ
            multiplierSpeed = speedModifier.GetMultiplierTarget();
        } else {
            // FadeOut フェーズ
            if (speedModifier.GetMultiplierFadeOutDuration() > 0.f) {
                speedModifier.SetMultiplierFadeOutTimer((std::min)(speedModifier.GetMultiplierFadeOutTimer() + deltaTime, speedModifier.GetMultiplierFadeOutDuration()));
                t               = speedModifier.GetMultiplierFadeOutTimer() / speedModifier.GetMultiplierFadeOutDuration();
                multiplierSpeed = std::lerp(speedModifier.GetMultiplierTarget(), 1.f, EasingFunctions[static_cast<int>(speedModifier.GetMultiplierFadeOutEaseType())](t));
            } else {
                multiplierSpeed = LerpByDeltaTime(speedModifier.GetMultiplierTarget(), 1.f, deltaTime, speedModifier.GetRestoreSpeed());
            }
        }

        Vec3f vel     = rigidbodyComp->GetVelocity();
        Vec3f velNorm = vel.normalize();
        float velLen  = vel.length();

        Vec3f effectiveSpeed;

        if (speedModifier.GetAxesSpace() == SpeedModifierAxesSpace::Velocity) {
            // --- 速度相対空間 (Front, Side, Up) ---
            // Front = 速度方向, Side = 水平垂直, Up = 右手系完成方向
            if (velLen < kEpsilon) {
                // 速度がほぼゼロの場合は変更なし
                effectiveSpeed = vel;
            } else {
                Vec3f worldUp = {0.f, 1.f, 0.f};
                Vec3f front   = velNorm;
                Vec3f side    = worldUp.cross(front); // 左手座標系: WorldUp × Front = Right
                if (side.lengthSq() < kEpsilon) {
                    side = {1.f, 0.f, 0.f}; // 真上/真下移動時のフォールバック
                } else {
                    side = side.normalize();
                }
                Vec3f up = front.cross(side); // 左手座標系: Front × Side = Up

                // 速度を各基底成分に分解 (front=velLen, side/up=0 が通常)
                float comps[3]           = {vel.dot(front), vel.dot(side), vel.dot(up)};
                const Vec3f basisVecs[3] = {front, side, up};

                const auto& mulAxes = speedModifier.GetMultiplierAxes();
                const auto& addAxes = speedModifier.GetAdditiveAxes();

                for (int i = 0; i < 3; ++i) {
                    float base  = mulAxes[i] ? speedModifier.GetBeforeSpeed() : comps[i];
                    float mul   = mulAxes[i] ? multiplierSpeed : 1.f;
                    float add   = addAxes[i] ? additiveSpeed : 0.f;
                    float scale = base * mul + add;
                    for (int k = 0; k < 3; ++k) {
                        effectiveSpeed[k] += scale * basisVecs[i][k];
                    }
                }
            }
        } else {
            // --- ワールド空間 (X, Y, Z) ---
            const auto& mulAxes = speedModifier.GetMultiplierAxes();
            const auto& addAxes = speedModifier.GetAdditiveAxes();

            for (int i = 0; i < 3; ++i) {
                float base        = mulAxes[i] ? speedModifier.GetBeforeSpeed() : velLen;
                float mul         = mulAxes[i] ? multiplierSpeed : 1.f;
                float add         = addAxes[i] ? additiveSpeed : 0.f;
                effectiveSpeed[i] = (base * mul + add) * velNorm[i];
            }
        }

        float maxSpeed = (std::max)(effectiveSpeed.length(), rigidbodyComp->GetMaxXZSpeed());
        rigidbodyComp->SetMaxXZSpeed(maxSpeed);
        rigidbodyComp->SetVelocity(effectiveSpeed);

        // 自動削除
        if (speedModifier.GetIsAutoDestroyed()) {
            if (!speedModifier.IsAdditiveEffectActive() && !speedModifier.IsMultiplierEffectActive()) {
                GetScene()->AddDeleteEntity(_handle);
            }
        }
    }
}
