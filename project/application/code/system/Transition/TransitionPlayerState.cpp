#include "TransitionPlayerState.h"

/// engine
#define DELTA_TIME
#define ENGINE_ECS
#include "EngineInclude.h"

#include "scene/SceneFactory.h"

#include "messageBus/MessageBus.h"

/// application
#include "manager/PlayerProgressStore.h"

// component
#include "component/scene/SceneChanger.h"
#include "component/TimerComponent.h"

#include "component/spline/TireSplinePoints.h"

#include "component/material/Material.h"

#include "component/stage/StageData.h"

#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerMoveStateFactory.h"
#include "component/player/state/PlayerState.h"

#include "component/Camera/CameraController.h"
#include "component/camera/CameraShakeSourceComponent.h"
#include "component/transform/CameraTransform.h"

/// event
#include "event/PlayerStateChangedEvent.h"

/// math
#include "math/MathEnv.h"
#include "math/MyEasing.h"

using namespace OriGine;

void TransitionPlayerState::UpdateEntity(EntityHandle _handle) {
    PlayerState* state   = GetComponent<PlayerState>(_handle);
    PlayerStatus* status = GetComponent<PlayerStatus>(_handle);
    Rigidbody* rigidbody = GetComponent<Rigidbody>(_handle);
    Transform* transform = GetComponent<Transform>(_handle);

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");

    /// =====================================================
    // StateUpdate
    /// =====================================================
    // goal 判定
    ///! TODO : 専用のシステムクラスで判定
    if (state->IsGoal()) {
        // clearTime Set
        PlayerProgressStore* progressStore = PlayerProgressStore::GetInstance();
        EntityHandle timerEntityHandle     = GetUniqueEntity("Timer");
        EntityHandle stageDataEntityHandle = GetUniqueEntity("StageData");
        if (timerEntityHandle.IsValid() && stageDataEntityHandle.IsValid()) {
            StageData* stageData  = GetComponent<StageData>(stageDataEntityHandle);
            TimerComponent* timer = GetComponent<TimerComponent>(timerEntityHandle);

            if (timer != nullptr && stageData != nullptr) {
                progressStore->StageCleared(
                    stageData->GetStageNumber(),
                    timer->GetTime());
            }
        }

        // sceneChange
        SceneChanger* sceneChanger = GetComponent<SceneChanger>(_handle);
        if (sceneChanger) {
            sceneChanger->ChangeScene();
            return;
        }
    }

    /// =====================================================
    // PlayerMoveState の初期化 (必要なら)
    /// =====================================================
    if (!state->GetPlayerMoveState()) {
        state->GetStateEnumRef().Set(PlayerMoveState::IDLE);
        state->SetPlayerMoveState(CreatePlayerMoveStateByEnum(PlayerMoveState::IDLE, this->GetScene(), _handle));
    }

    /// =====================================================
    // MoveStateUpdate
    /// =====================================================
    auto& moveStateFlag = state->GetStateEnumRef();
    moveStateFlag.Set(state->GetPlayerMoveState()->TransitionState());

    // 状態が変わった場合、状態を更新
    if (moveStateFlag.IsChanged()) {
        state->GetPlayerMoveState()->Finalize();

        state->SetPlayerMoveState(CreatePlayerMoveStateByEnum(moveStateFlag.Current().ToEnum(), this->GetScene(), _handle));
        state->GetPlayerMoveState()->Initialize();

        // event を発行
        PlayerStateChangedEvent event;
        event.playerEntityHandle = _handle;
        event.previousMoveState  = moveStateFlag.Prev().ToEnum();
        event.currentMoveState   = moveStateFlag.Current().ToEnum();
        MessageBus::GetInstance()->Emit<PlayerStateChangedEvent>(event);
    }

    PlayerEffectControlParam* effectParam = GetComponent<PlayerEffectControlParam>(_handle);
    if (effectParam && !effectParam->GetTireTrailSplineEntityHandle().IsValid()) {
        if (moveStateFlag.Current().ToEnum() == PlayerMoveState::DASH) {
            // タイヤトレイルを作る
            constexpr char kPlayerTireTrailEntityName[] = "PlayerTireTrail";
            SceneFactory sceneFactory;
            Entity* tierTrail = sceneFactory.BuildEntityFromTemplate(this->GetScene(), kPlayerTireTrailEntityName);
            if (tierTrail) {
                auto tireSplinePoints = GetComponent<TireSplinePoints>(tierTrail->GetHandle());
                if (tireSplinePoints) {
                    tireSplinePoints->commonSettings.playerEntityHandle = _handle;
                    effectParam->SetTireTrailSplineEntityId(tierTrail->GetHandle());
                }
            }
        }
    }

    // state Update
    EnumBitmask<PlayerStateFlag> newFlag = state->GetStateFlagRef().Current();
    newFlag.ClearFlag(PlayerStateFlag::GEAR_UP);
    newFlag.ClearFlag(PlayerStateFlag::IS_GOAL);
    newFlag.ClearFlag(PlayerStateFlag::IS_PENALTY);
    newFlag.ClearFlag(PlayerStateFlag::IS_RESTART);

    state->GetStateFlagRef().Set(newFlag);

    status->UpdateWallRunInterval(deltaTime);
    status->UpdateRailInterval(deltaTime);

    // ペナルティ時間 更新
    state->SubtractPenaltyTime(deltaTime);
    float currentInvisibleTime = state->GetInvincibilityTime();
    if (currentInvisibleTime > 0.f) {
        state->SubtractInvincibilityTime(deltaTime);

        // animation 更新
        Material* material = GetComponent<Material>(_handle);
        // 残り時間の割合 (1.0 → 0.0 に減少)
        float t = currentInvisibleTime / status->GetInvincibilityTime();

        // 点滅の振幅を残り時間に応じて変化させる
        // t が小さくなる(終わりに近づく)ほど amplitude が大きくなる
        float baseAmplitude = effectParam->GetInvincibleBlinkBaseAmplitude();
        float maxAmplitude  = effectParam->GetInvincibleBlinkMaxAmplitude();
        float amplitude     = std::lerp(maxAmplitude, baseAmplitude, t);

        // 経過時間を使って点滅を計算（残り時間ではなく経過時間のほうが安定）
        float elapsedTime = status->GetInvincibilityTime() - currentInvisibleTime;
        // 0~1 の間で 点滅させる
        material->color_[A] = std::clamp(EaseInSine((sinf(elapsedTime * amplitude) * 0.5f) + 0.5f), 0.f, 1.f);
    } else {
        Material* material  = GetComponent<Material>(_handle);
        material->color_[A] = 1.f;
    }

    /// =====================================================
    // Fov Y
    /// =====================================================
    OriGine::Entity* gameCamera = GetEntity(state->GetCameraEntityHandle());
    if (!gameCamera) {
        return;
    }
    CameraController* cameraController = GetComponent<CameraController>(gameCamera->GetHandle());
    if (cameraController) {
        // fov 更新
        CameraTransform* cameraTransform = GetComponent<CameraTransform>(gameCamera->GetHandle());
        if (cameraTransform) {
            const float xzSpeed        = Vec2f(rigidbody->GetVelocity(X), rigidbody->GetVelocity(Z)).length();
            cameraTransform->fovAngleY = std::lerp(cameraTransform->fovAngleY, cameraController->CalculateFovYBySpeed(xzSpeed), cameraController->fovYInterpolate);
        };
    }

    // 着地時のカメラシェイク
    auto shakeSource = GetComponent<CameraShakeSourceComponent>(gameCamera->GetHandle(), 1);
    if (shakeSource) {
        if (state->IsJustLanded()) {
            constexpr float kShakeThreshold = 0.184f; // シェイクを発生させる最低着地衝撃値
            constexpr float kShakeScale     = 1.31f; // 着地衝撃をシェイクの強さに変換するスケール

            const Vec3f& prePos = rigidbody->GetPrePos();
            const Vec3f& pos    = transform->GetWorldTranslate();
            float deltaY        = prePos[Y] - pos[Y];

            if (std::abs(deltaY) < kShakeThreshold) {
                return;
            }
            shakeSource->StartShake();
            shakeSource->springVelocity[Y] = -deltaY * kShakeScale;
        }
    }
}
