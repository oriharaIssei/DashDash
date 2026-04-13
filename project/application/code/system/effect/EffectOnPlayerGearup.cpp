#include "EffectOnPlayerGearup.h"

/// Engine
#define ENGINE_ECS
#define DELTA_TIME
#include "EngineInclude.h"

#include "scene/SceneFactory.h"

// component
#include "component/animation/MaterialAnimation.h"
#include "component/material/Material.h"

#include "component/renderer/primitive/RingRenderer.h"

#include "component/effect/particle/emitter/ParticleSystem.h"
#include "component/effect/post/DistortionEffectParam.h"

#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

#include "component/player/PlayerConfig.h"

/// math
#include <math/MyEasing.h>

using namespace OriGine;

EffectOnPlayerGearup::EffectOnPlayerGearup() : ISystem(OriGine::SystemCategory::Effect) {}

EffectOnPlayerGearup::~EffectOnPlayerGearup() {}

void EffectOnPlayerGearup::Initialize() {}

void EffectOnPlayerGearup::Finalize() {}

void EffectOnPlayerGearup::UpdateEntity(OriGine::EntityHandle _handle) {
    OriGine::EntityHandle player = GetUniqueEntity("Player");

    PlayerState* state                  = GetComponent<PlayerState>(player);
    OriGine::Transform* playerTransform = GetComponent<OriGine::Transform>(player);

    if (!state || !playerTransform) {
        return;
    }

    SceneFactory factory;

    /// ==============================
    // Effect PlayState Transition
    /// ==============================

    shockWaveState_.playState.Sync();
    if (state->IsGearUp()) {
        shockWaveState_.playState.SetCurrent(false);

        OriGine::Transform* transform = GetComponent<OriGine::Transform>(_handle);
        transform->translate          = playerTransform->translate; // Playerの位置に合わせる

        // Emitterの再生
        auto& emitters = GetComponents<ParticleSystem>(_handle);
        for (auto& emitter : emitters) {
            emitter.SetOriginPos(emitterOffset_);
            emitter.PlayStart();
        }

        // GearLevelに応じて衝撃波を発生
        if (state->GetGearLevel() >= AppConfig::Player::kGearUpEffectShockWaveThresholdGearLevel) {
            shockWaveState_.playState.SetCurrent(true);
            shockWaveState_.currentTime = 0.f;
        }

        // trailの色をGearLevelに応じて変化
        auto* effectControlParam = GetComponent<PlayerEffectControlParam>(player);

        if (effectControlParam) {
            OriGine::Vec4f trailColor = effectControlParam->GetTrailColorByGearLevel(state->GetGearLevel());

            OriGine::EntityHandle backFireEntity         = GetUniqueEntity("BackFire");
            MaterialAnimation* backFireMaterialAnimation = GetComponent<MaterialAnimation>(backFireEntity);

            if (backFireMaterialAnimation) {
                for (auto& colorKey : backFireMaterialAnimation->GetColorCurve()) {
                    // alpha値は変えない
                    // rgbは trailColor に合わせる
                    colorKey = Keyframe(colorKey.time, Vec4f(trailColor[X], trailColor[Y], trailColor[Z], colorKey.value[W]));
                }
                // アニメーションを最初から再生
                backFireMaterialAnimation->PlayStart();
            }

            // chargeEffectを生成
            Entity* chargeEffectEntity = factory.BuildEntityFromTemplate(GetScene(), "ChargeEffect");
            if (chargeEffectEntity) {
                EntityHandle chargeEffectHandle  = chargeEffectEntity->GetHandle();
                Transform* chargeEffectTransform = GetComponent<Transform>(chargeEffectHandle);
                chargeEffectTransform->translate = Vec3f(0.f, 0.f, 0.f);
                chargeEffectTransform->parent    = playerTransform;

                auto chargeMaterialEffect = GetComponent<MaterialAnimation>(chargeEffectHandle);
                if (chargeMaterialEffect) {
                    for (auto& colorKey : chargeMaterialEffect->GetColorCurve()) {
                        // alpha値は変えない
                        // rgbは trailColor に合わせる
                        colorKey = Keyframe(colorKey.time, Vec4f(trailColor[X], trailColor[Y], trailColor[Z], colorKey.value[W]));
                    }
                    // アニメーションを最初から再生
                    chargeMaterialEffect->PlayStart();
                }
            }
        }
    }

    UpdateShockWaveRing(_handle, playerTransform);
}

void EffectOnPlayerGearup::UpdateShockWaveRing(OriGine::EntityHandle _handle, OriGine::Transform* _playerTransform) {
    DistortionEffectParam* distortionEffectParam = GetComponent<DistortionEffectParam>(_handle);

    auto& shockWaveRings  = distortionEffectParam->GetDistortionObjects();
    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");

    // 更新時 以外
    if (shockWaveState_.playState.Current()) { /// shockWave Play

        // 初期化処理
        if (shockWaveState_.playState.IsTrigger()) {

            // shockWaveRingの初期化Add commentMore actions
            for (auto& [object, type] : shockWaveRings) {
                if (type != PrimitiveType::Ring) {
                    continue;
                }
                if (object == nullptr) {
                    continue;
                }

                RingRenderer* ringRenderer = dynamic_cast<RingRenderer*>(object.get());

                ringRenderer->GetTransform().translate    = _playerTransform->translate + (shockWaveOffset_ * MakeMatrix4x4::RotateY(_playerTransform->rotate.ToYaw())); // 初期位置を設定
                ringRenderer->GetTransform().rotate       = _playerTransform->rotate;
                ringRenderer->GetPrimitive().innerRadius_ = minInnerRadius_;
                ringRenderer->GetPrimitive().outerRadius_ = minOuterRadius_;
                ringRenderer->GetTransform().UpdateMatrix();
            }

            shockWaveState_.currentTime = 0.f; // 時間をリセット
        }

        shockWaveState_.currentTime += deltaTime;
        float t = shockWaveState_.currentTime / shockWaveState_.maxTime;

        if (t >= 1.f) {
            t = 1.f;
            shockWaveState_.playState.Set(false);
        }

        float easeT = EaseInQuad(t);

        // 衝撃波の拡大
        for (auto& [object, type] : shockWaveRings) {
            if (type != PrimitiveType::Ring) {
                continue;
            }
            if (object == nullptr) {
                continue;
            }
            RingRenderer* ringRenderer = dynamic_cast<RingRenderer*>(object.get());

            float innerRadius                         = std::lerp(minInnerRadius_, maxInnerRadius_, easeT);
            float outerRadius                         = std::lerp(minOuterRadius_, maxOuterRadius_, easeT);
            ringRenderer->GetPrimitive().innerRadius_ = innerRadius;
            ringRenderer->GetPrimitive().outerRadius_ = outerRadius;
            auto* mesh                                = &ringRenderer->GetMeshGroup()->back();
            ringRenderer->CreateMesh(mesh);
        }
    } else if (!shockWaveState_.playState.IsRelease()) {
        // shockWaveRingのを見えない位置に隠す
        for (auto& [object, type] : shockWaveRings) {
            if (type != PrimitiveType::Ring) {
                continue;
            }
            if (object == nullptr) {
                continue;
            }

            RingRenderer* ringRenderer = dynamic_cast<RingRenderer*>(object.get());

            // 初期位置
            constexpr float kHidePositionY            = -10000.f;
            ringRenderer->GetTransform().translate[Y] = kHidePositionY;
            ringRenderer->GetTransform().UpdateMatrix();
        }
    }
}
