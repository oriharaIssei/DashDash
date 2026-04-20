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
#include "IShakeStrategy.h"

using namespace OriGine;

//=============================================================================
// Factory
//=============================================================================

static IShakeStrategy* GetShakeStrategy(ShakeSourceType _type) {
    static SinCurveShakeStrategy sinCurve;
    static NoiseShakeStrategy noise;
    static SpringShakeStrategy spring;

    switch (_type) {
    case ShakeSourceType::SinCurve: return &sinCurve;
    case ShakeSourceType::Noise:    return &noise;
    case ShakeSourceType::Spring:   return &spring;
    default:                        return nullptr;
    }
}

//=============================================================================
// CameraShake system
//=============================================================================

CameraShake::CameraShake() : ISystem(OriGine::SystemCategory::Effect) {}
CameraShake::~CameraShake() {}

void CameraShake::Initialize() {}
void CameraShake::Finalize() {}

void CameraShake::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& cameraShakeSources = GetComponents<CameraShakeSourceComponent>(_handle);

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Effect");

    if (cameraShakeSources.empty()) {
        return;
    }
    for (auto& cameraShakeSource : cameraShakeSources) {
        auto cameraTransform = GetComponent<CameraTransform>(_handle, cameraShakeSource.cameraTransformIndex);
        if (!cameraTransform) {
            continue;
        }

        if (!cameraShakeSource.isActive) {
            continue;
        }

        cameraShakeSource.elapsedTime += deltaTime;
        if (!cameraShakeSource.isLoop) {
            if (cameraShakeSource.duration <= cameraShakeSource.elapsedTime) {
                cameraShakeSource.isActive = false;
                continue;
            }
        }

        IShakeStrategy* strategy = GetShakeStrategy(cameraShakeSource.type);
        if (!strategy) {
            continue;
        }

        Vec3f shakeOffset = strategy->Calculate(cameraShakeSource, deltaTime);

        cameraTransform->translate += shakeOffset;
        cameraTransform->UpdateMatrix();
        CameraManager::GetInstance()->SetTransform(GetScene(), *cameraTransform);
    }
}
