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
        auto rigidbodyComp = GetComponent<Rigidbody>(speedModifier.targetRigidbodyHandle);
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

        speedModifier.additiveTimer += deltaTime;
        speedModifier.additiveFadeInTimer += deltaTime;
        if (speedModifier.additiveFadeInTimer <= speedModifier.additiveFadeInDuration) {
            // FadeIn フェーズ
            t             = (std::min)(speedModifier.additiveFadeInTimer / speedModifier.additiveFadeInDuration, 1.0f);
            additiveSpeed = std::lerp(0.f, speedModifier.additiveTarget, EasingFunctions[static_cast<int>(speedModifier.additiveLerpEaseType)](t));
        } else if (speedModifier.additiveTimer < speedModifier.additiveDuration) {
            // Hold フェーズ
            additiveSpeed = speedModifier.additiveTarget;
        } else {
            // FadeOut フェーズ
            if (speedModifier.additiveFadeOutDuration > 0.f) {
                speedModifier.additiveFadeOutTimer = (std::min)(speedModifier.additiveFadeOutTimer + deltaTime, speedModifier.additiveFadeOutDuration);
                t                                  = speedModifier.additiveFadeOutTimer / speedModifier.additiveFadeOutDuration;
                additiveSpeed                      = std::lerp(speedModifier.additiveTarget, 0.f, EasingFunctions[static_cast<int>(speedModifier.additiveFadeOutEaseType)](t));
            } else {
                additiveSpeed = LerpByDeltaTime(speedModifier.additiveTarget, 0.f, deltaTime, speedModifier.restoreSpeed);
            }
        }

        speedModifier.multiplierTimer += deltaTime;
        speedModifier.multiplierFadeInTimer += deltaTime;
        if (speedModifier.multiplierFadeInTimer <= speedModifier.multiplierFadeInDuration) {
            // FadeIn フェーズ
            t               = (std::min)(speedModifier.multiplierFadeInTimer / speedModifier.multiplierFadeInDuration, 1.0f);
            multiplierSpeed = std::lerp(1.f, speedModifier.multiplierTarget, EasingFunctions[static_cast<int>(speedModifier.multiplierLerpEaseType)](t));
        } else if (speedModifier.multiplierTimer < speedModifier.multiplierDuration) {
            // Hold フェーズ
            multiplierSpeed = speedModifier.multiplierTarget;
        } else {
            // FadeOut フェーズ
            if (speedModifier.multiplierFadeOutDuration > 0.f) {
                speedModifier.multiplierFadeOutTimer = (std::min)(speedModifier.multiplierFadeOutTimer + deltaTime, speedModifier.multiplierFadeOutDuration);
                t                                    = speedModifier.multiplierFadeOutTimer / speedModifier.multiplierFadeOutDuration;
                multiplierSpeed                      = std::lerp(speedModifier.multiplierTarget, 1.f, EasingFunctions[static_cast<int>(speedModifier.multiplierFadeOutEaseType)](t));
            } else {
                multiplierSpeed = LerpByDeltaTime(speedModifier.multiplierTarget, 1.f, deltaTime, speedModifier.restoreSpeed);
            }
        }

        Vec3f vel     = rigidbodyComp->GetVelocity();
        Vec3f velNorm = vel.normalize();
        float velLen  = vel.length();

        Vec3f effectiveSpeed;

        if (speedModifier.axesSpace == SpeedModifiers::AxesSpace::Velocity) {
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

                for (int i = 0; i < 3; ++i) {
                    float base  = speedModifier.multiplierAxes[i] ? speedModifier.beforeSpeed : comps[i];
                    float mul   = speedModifier.multiplierAxes[i] ? multiplierSpeed : 1.f;
                    float add   = speedModifier.additiveAxes[i] ? additiveSpeed : 0.f;
                    float scale = base * mul + add;
                    for (int k = 0; k < 3; ++k) {
                        effectiveSpeed[k] += scale * basisVecs[i][k];
                    }
                }
            }
        } else {
            // --- ワールド空間 (X, Y, Z) ---
            for (int i = 0; i < 3; ++i) {
                float base        = speedModifier.multiplierAxes[i] ? speedModifier.beforeSpeed : velLen;
                float mul         = speedModifier.multiplierAxes[i] ? multiplierSpeed : 1.f;
                float add         = speedModifier.additiveAxes[i] ? additiveSpeed : 0.f;
                effectiveSpeed[i] = (base * mul + add) * velNorm[i];
            }
        }

        float maxSpeed = (std::max)(effectiveSpeed.length(), rigidbodyComp->GetMaxXZSpeed());
        rigidbodyComp->SetMaxXZSpeed(maxSpeed);
        rigidbodyComp->SetVelocity(effectiveSpeed);

        // 自動削除
        if (speedModifier.isAutoDestroyed) {
            if (!speedModifier.IsAdditiveEffectActive() && !speedModifier.IsMultiplierEffectActive()) {
                GetScene()->AddDeleteEntity(_handle);
            }
        }
    }
}
