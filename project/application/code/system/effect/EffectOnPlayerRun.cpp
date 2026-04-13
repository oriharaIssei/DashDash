#include "EffectOnPlayerRun.h"

#include "EffectConfig.h"

/// Engine
#define DELTA_TIME
#include "EngineInclude.h"

/// ECS
// component
#include "component/renderer/ModelMeshRenderer.h"
#include "component/renderer/primitive/CylinderRenderer.h"

#include "component/effect/post/SpeedlineEffectParam.h"
#include "component/player/PlayerEffectControlParam.h"

#include "component/animation/MaterialAnimation.h"

#include "component/physics/Rigidbody.h"
#include "component/transform/Transform.h" 

#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

/// math
#include "math/Interpolation.h"
#include "math/MathEnv.h"
#include "math/MyEasing.h"

using namespace OriGine;

EffectOnPlayerRun::EffectOnPlayerRun() : ISystem(OriGine::SystemCategory::Effect) {}
EffectOnPlayerRun::~EffectOnPlayerRun() {}

void EffectOnPlayerRun::Initialize() {}
void EffectOnPlayerRun::Finalize() {}

void EffectOnPlayerRun::UpdateEntity(EntityHandle _entity) {
    auto state              = GetComponent<PlayerState>(_entity);
    auto status             = GetComponent<PlayerStatus>(_entity);
    auto effectControlParam = GetComponent<PlayerEffectControlParam>(_entity);
    auto rigidbody          = GetComponent<Rigidbody>(_entity);

    if (!state || !status || !effectControlParam || !rigidbody) {
        return;
    }
    float deltaTime              = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");
    const Vec3f& currentVelocity = rigidbody->GetVelocity();
    float currentXZSpeed         = Vec2f(currentVelocity[X], currentVelocity[Z]).length();
    float playerMaxSpeed         = status->CalculateSpeedByGearLevel(kMaxPlayerGearLevel);

    // タイヤの回転
    ModelMeshRenderer* modelMeshRenderer = GetComponent<ModelMeshRenderer>(_entity);
    if (modelMeshRenderer) {
        Transform& meshTransform = modelMeshRenderer->GetTransformRef(0);

        auto playerInput        = GetComponent<PlayerInput>(_entity);
        OriGine::Vec3f inputDir = playerInput->GetWorldInputDirection();

        // 速度によって タイヤの回転速度を変える
        // 入力方向ベクトルが0のときは 回転しないようにする(Sqなのは,0か1しかないので軽くするため)
        float wheelSpinSpeed = effectControlParam->CalculateWheelSpinSpeedBySpeed(currentVelocity.length() * inputDir.lengthSq(), playerMaxSpeed);
        meshTransform.rotate *= Quaternion::RotateAxisAngle(axisX, wheelSpinSpeed);
        meshTransform.UpdateMatrix();

        // プレイヤーが曲がる時,タイヤを傾ける (地面にいるときだけ)
        Transform* hostTransform = GetComponent<OriGine::Transform>(_entity);
        if (state->IsOnGround()) {
            if (inputDir.lengthSq() > kEpsilon) {
                OriGine::Vec3f currentDir = hostTransform->rotate.RotateVector(axisZ);
                currentDir[Y]             = 0.f;
                currentDir                = currentDir.normalize();

                // 現在の傾き
                float wheelTiltAngle = effectControlParam->CalculateWheelTiltAngle(inputDir, currentDir);

                // 段々と傾く(傾く角度が制限されている)
                float preWheelTiltAngle = effectControlParam->GetPreWheelTiltAngle();
                float angleDiff         = wheelTiltAngle - preWheelTiltAngle;
                float maxAngleChange    = effectControlParam->GetWheelTiltAngleMaxAccel() * deltaTime;
                wheelTiltAngle          = preWheelTiltAngle + std::clamp(angleDiff, -maxAngleChange, maxAngleChange);

                wheelTiltAngle = std::clamp(wheelTiltAngle, -effectControlParam->GetMaxWheelTiltAngle(), effectControlParam->GetMaxWheelTiltAngle());

                // 傾きを適用
                hostTransform->rotate *= Quaternion::RotateAxisAngle(axisZ, wheelTiltAngle);
                hostTransform->UpdateMatrix();

                effectControlParam->SetPreWheelTiltAngle(wheelTiltAngle);
            }
        } else {
            effectControlParam->SetPreWheelTiltAngle(0.f);
        }
    }

    // trailの色をGearLevelに応じて変化
    if (effectControlParam) {
        // BackFire を 速度によって 強さを変える
        constexpr float kMinBackFireScaleY  = EffectConfig::BackFire::kMinScaleY;
        constexpr float kMaxBackFireScaleY  = EffectConfig::BackFire::kMaxScaleY;
        constexpr float kMinBackFireScaleXZ = EffectConfig::BackFire::kMinScaleXZ;
        constexpr float kMaxBackFireScaleXZ = EffectConfig::BackFire::kMaxScaleXZ;

        constexpr int32_t trailAnimationOnGearUpIndex = 1;
        const OriGine::Vec4f& trailColor              = effectControlParam->GetTrailColorByGearLevel(state->GetGearLevel());

        OriGine::EntityHandle trailEntity = GetUniqueEntity("Trail");
        auto* trailMaterialAnimation      = GetComponent<MaterialAnimation>(trailEntity, trailAnimationOnGearUpIndex);

        // 色をGearLevelに応じて変化
        // アニメーションが再生されていなければ 色を設定
        if (trailMaterialAnimation && !trailMaterialAnimation->GetAnimationIsPlay()) {
            auto* material = GetComponent<Material>(trailEntity);
            if (material) {
                material->color_ = trailColor;
            }
        }

        OriGine::EntityHandle backFireEntity         = GetUniqueEntity("BackFire");
        MaterialAnimation* backFireMaterialAnimation = GetComponent<MaterialAnimation>(backFireEntity);
        // animationが再生されていなければ 色を設定
        if (backFireMaterialAnimation && !backFireMaterialAnimation->GetAnimationIsPlay()) {
            auto* material = GetComponent<Material>(backFireEntity);
            if (material) {
                material->color_ = trailColor;
            }
        }

        OriGine::EntityHandle backFireSparksEntity = GetUniqueEntity("BackFireSparks");
        auto& backFireCylinders                    = GetComponents<CylinderRenderer>(backFireEntity);
        auto& backFireSparksCylinders              = GetComponents<CylinderRenderer>(backFireSparksEntity);

        float t       = currentXZSpeed / playerMaxSpeed;
        t             = EaseInCubic(t);
        float xzScale = std::lerp(kMinBackFireScaleXZ, kMaxBackFireScaleXZ, t);
        float yScale  = std::lerp(kMinBackFireScaleY, kMaxBackFireScaleY, t);

        OriGine::Vec3f newScale = OriGine::Vec3f(xzScale, yScale, xzScale);
        for (auto& cylinder : backFireCylinders) {
            cylinder.GetTransform().scale = newScale;
            cylinder.GetTransform().UpdateMatrix();
        }
        for (auto& cylinder : backFireSparksCylinders) {
            cylinder.GetTransform().scale = newScale;
            cylinder.GetTransform().UpdateMatrix();
        }
    }

    // スピードラインエフェクト
    OriGine::EntityHandle speedlineEntity = GetUniqueEntity("Speedline");
    auto& speedlineParams                 = GetComponents<SpeedlineEffectParam>(speedlineEntity);

    // プレイヤーが止まっているか, ギアレベルが低い場合は エフェクトを停止
    if (state->GetStateEnum() == PlayerMoveState::IDLE || state->GetGearLevel() < 2) {
        for (auto& speedlineParam : speedlineParams) {
            speedlineParam.Stop();
        }
    } else {
        // 速度によって エフェクトの強さを変える
        float intensityT = currentXZSpeed / playerMaxSpeed;
        intensityT       = EaseOutCubic(intensityT);
        float intensity  = std::lerp(0.f, kMaxIntensity_, intensityT);

        for (auto& speedlineParam : speedlineParams) {
            speedlineParam.Play();
            auto& paramData = speedlineParam.GetParamData();
            paramData.time -= deltaTime;
            paramData.intensity = std::lerp(paramData.intensity, intensity, 0.1f);
        }
    }
}
