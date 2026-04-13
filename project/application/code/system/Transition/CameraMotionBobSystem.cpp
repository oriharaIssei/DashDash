#include "CameraMotionBobSystem.h"

/// ECS
// component
#include "component/camera/CameraMotionBob.h"
#include "component/camera/CameraShakeSourceComponent.h"
#include "component/player/State/PlayerState.h"

using namespace OriGine;

CameraMotionBobSystem::CameraMotionBobSystem() : ISystem(OriGine::SystemCategory::StateTransition) {}

void CameraMotionBobSystem::Initialize() {}
void CameraMotionBobSystem::Finalize() {}

void CameraMotionBobSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    OriGine::EntityHandle playerEnt = GetUniqueEntity("Player");
    auto* playerState               = GetComponent<PlayerState>(playerEnt);
    // Player の Stateがなければ 実行しない
    if (!playerState) {
        return;
    }

    // CameraMotionBobComponentを取得
    auto& motionBobComps = GetComponents<CameraMotionBob>(_handle);
    if (motionBobComps.empty()) {
        return;
    }

    // カメラの揺れをするかどうか
    for (auto& motionBob : motionBobComps) {
        // shakesource を取得
        int32_t shakeSourceCompIndex = motionBob.cameraShakeSourceComponentId;
        auto* shakeSourceComp        = GetComponent<CameraShakeSourceComponent>(_handle, shakeSourceCompIndex);
        if (!shakeSourceComp) {
            continue;
        }

        int32_t playerGearLevel   = playerState->GetGearLevel();
        bool isShake              = motionBob.thresholdGearLevel < playerGearLevel;
        shakeSourceComp->isActive = isShake;

        // カメラシェイクしないなら スキップ
        if (!isShake) {
            continue;
        }

        // 線形補間
        float levelT             = EaseOutQuad(static_cast<float>(playerGearLevel - motionBob.thresholdGearLevel) / static_cast<float>(kMaxPlayerGearLevel - motionBob.thresholdGearLevel));
        OriGine::Vec3f amplitude = Lerp<3, float>(motionBob.minAmplitude, motionBob.maxAmplitude, levelT);
        OriGine::Vec3f frequency = Lerp<3, float>(motionBob.minFrequency, motionBob.maxFrequency, levelT);

        // 適応
        for (size_t i = 0; i < 3; ++i) {
            shakeSourceComp->axisParameters[i].amplitude = amplitude[i];
            shakeSourceComp->axisParameters[i].frequency = frequency[i];
        }
    }
}
