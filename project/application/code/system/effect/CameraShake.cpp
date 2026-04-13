#include "CameraShake.h"

/// engine
#include "Engine.h"

#include "camera/CameraManager.h"

/// ECS
// component
#include "component/Camera/CameraShakeSourceComponent.h"
#include "component/transform/CameraTransform.h"

/// math
#include "math/MathEnv.h"
#include "math/Noise.h"

#include "EffectConfig.h"

using namespace OriGine;

CameraShake::CameraShake() : ISystem(OriGine::SystemCategory::Effect) {}
CameraShake::~CameraShake() {}

void CameraShake::Initialize() {}
void CameraShake::Finalize() {}

void CameraShake::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& cameraShakeSources = GetComponents<CameraShakeSourceComponent>(_handle);

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Effect");

    // CameraShakeSourceComponentが存在しない場合は Skip
    if (cameraShakeSources.empty()) {
        return;
    }
    for (auto& cameraShakeSource : cameraShakeSources) {
        // シェイクの適用先カメラTransformを取得
        auto cameraTransform = GetComponent<CameraTransform>(_handle, cameraShakeSource.cameraTransformIndex);
        if (!cameraTransform) {
            continue;
        }

        if (!cameraShakeSource.isActive) {
            continue;
        }

        // シェイクの継続時間処理
        cameraShakeSource.elapsedTime += deltaTime;
        if (!cameraShakeSource.isLoop) {
            if (cameraShakeSource.duration <= cameraShakeSource.elapsedTime) {
                // シェイク終了
                cameraShakeSource.isActive = false;
                continue;
            }
        }

        // シェイクオフセット計算
        OriGine::Vec3f shakeOffset(0.0f, 0.0f, 0.0f);
        switch (cameraShakeSource.type) {
        case ShakeSourceType::SinCurve:
            for (size_t i = 0; i < 3; ++i) {
                shakeOffset[i] = sinf(cameraShakeSource.elapsedTime * cameraShakeSource.axisParameters[i].frequency) * cameraShakeSource.axisParameters[i].amplitude;
            }
            break;
        case ShakeSourceType::Noise:
            for (size_t i = 0; i < 3; ++i) {
                float time = cameraShakeSource.elapsedTime * cameraShakeSource.axisParameters[i].frequency;

                OriGine::Vec2f st(
                    time, // 時間で動かす
                    time * EffectConfig::CameraShake::kNoiseCoefficient // 少しずらす。軸ごとに違う揺れになる
                );

                // ノイズを -1 ～ +1 に補正
                float f = FbmNoise::Fbm(st) - EffectConfig::CameraShake::kNoiseOffset;

                shakeOffset[i] = f * cameraShakeSource.axisParameters[i].amplitude;
            }
            break;
        case ShakeSourceType::Spring:
            for (size_t i = 0; i < 3; ++i) {
                // ω = 2π * frequency (固有角振動数)
                float omega = OriGine::kTau * cameraShakeSource.axisParameters[i].frequency;
                // 減衰比 ζ (0 < ζ < 1 で振動する)
                float zeta = cameraShakeSource.axisParameters[i].dampingRatio;

                // バネ-ダンパー加速度: a = -ω²x - 2ζωv
                float acc = -(omega * omega) * cameraShakeSource.springPosition[i]
                            - 2.0f * zeta * omega * cameraShakeSource.springVelocity[i];
                cameraShakeSource.springVelocity[i] += acc * deltaTime;
                cameraShakeSource.springPosition[i] += cameraShakeSource.springVelocity[i] * deltaTime;

                shakeOffset[i] = cameraShakeSource.springPosition[i];
            }
            // バネのエネルギーが十分小さい場合は自動停止 (Loop == false の場合のみ)
            if (!cameraShakeSource.isLoop && cameraShakeSource.springPosition.length() < kEpsilon && cameraShakeSource.springVelocity.length() < kEpsilon) {
                cameraShakeSource.isActive = false;
            }
            break;
        default:
            break;
        }

        // シェイクオフセット適用
        cameraTransform->translate += shakeOffset;
        cameraTransform->UpdateMatrix();
        CameraManager::GetInstance()->SetTransform(GetScene(), *cameraTransform);
    }
}
