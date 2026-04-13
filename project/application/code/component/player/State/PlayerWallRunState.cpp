#include "PlayerWallRunState.h"

/// engine
#include "scene/SceneFactory.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "component/renderer/ModelMeshRenderer.h"
#include "component/spline/SplinePoints.h"
#include "component/transform/Transform.h"

#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

/// log
#include "logger/Logger.h"

/// util
#include "component/player/PlayerConfig.h"
#include "component/player/PlayerMoveUtils.h"
#include "util/globalVariables/SerializedField.h"

/// math
#include "Interpolation.h"
#include "math/MathEnv.h"
#include "MyEasing.h"

using namespace OriGine;

namespace {
constexpr float kOffsetRate = AppConfig::Player::kWallRunOffsetRate;
}

void PlayerWallRunState::Initialize() {
    constexpr int32_t thresholdGearLevel = AppConfig::Player::kWallRunThresholdGearLevel;
    constexpr float kMeshOffsetRate      = AppConfig::Player::kWallRunMeshOffsetRate;

    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* state        = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 壁ジャンプ前の速度
    prevVelo_ = rigidbody->GetVelocity();

    // 壁法線
    wallNormal_ = state->GetWallCollisionNormal().normalize();

    // ===== 進行方向を「速度を壁面に投影して」求める =====
    OriGine::Vec3f direction = PlayerMoveUtils::ComputeWallRunDirection(prevVelo_, wallNormal_);
    direction[Y]             = 0.0f;

    if (direction.lengthSq() > kEpsilon) {
        direction = direction.normalize();
    } else {
        // フォールバック（ほぼ起きない）
        direction = axisZ * MakeMatrix4x4::RotateY(transform->rotate.ToYaw());
    }

    // ===== スピード =====
    playerSpeed_ = playerStatus->CalculateSpeedByGearLevel(state->GetGearLevel());

    // 基準レベル未満ならギアアップ
    int32_t gearLevel = state->GetGearLevel();
    if (gearLevel < thresholdGearLevel) {
        auto& stateFlag = state->GetStateFlagRef();
        stateFlag.SetCurrent(stateFlag.Current() | PlayerStateFlag::GEAR_UP);

        int32_t addedGearLevel = gearLevel + 1;
        state->SetGearLevel(addedGearLevel);

        playerStatus->SetGearUpCoolTime(
            playerStatus->CalculateCoolTimeByGearLevel(addedGearLevel));

        playerStatus->SetCurrentMaxSpeed(
            playerStatus->CalculateSpeedByGearLevel(addedGearLevel));

        playerSpeed_ = playerStatus->GetCurrentMaxSpeed();
    }

    // ===== 移動 =====
    rigidbody->SetVelocity(playerSpeed_ * direction);

    rigidbody->SetMass(playerStatus->GetMassOnWallRun());

    // 壁との分離猶予
    separationLeftTime_ = separationGraceTime_;
    // 壁走り離脱速度
    wallRunDetachSpeed_ = playerStatus->GetWallRunDetachSpeed();

    // ===== 向きとロール =====
    PlayerEffectControlParam* effectParam = scene_->GetComponent<PlayerEffectControlParam>(playerEntityHandle_);
    isRightWall_                          = PlayerMoveUtils::IsWallRight(direction, wallNormal_);

    float rotateZOffsetOnWallRun = effectParam->GetRotateOffsetOnWallRun();
    // プレイヤーの向きを移動方向に合わせる
    Quaternion lookForward = Quaternion::LookAt(direction, axisY);
    // 回転アニメーションのゴール地点を設定
    Quaternion angleOffset = Quaternion::RotateAxisAngle(axisZ, isRightWall_ ? rotateZOffsetOnWallRun : -rotateZOffsetOnWallRun);
    transform->rotate      = lookForward * angleOffset;

    // ===== スピード制御 =====
    speedRate_        = playerStatus->GetWallRunRate();
    speedRumpUpTime_  = playerStatus->GetWallRunRampUpTime();
    speedRumpUpTimer_ = 0.0f;

    // ===== 重力制御 =====
    gravityApplyDelay_ = playerStatus->GetGravityApplyDelayOnWallRun();
    rigidbody->SetUseGravity(false);

    // ===== メッシュのオフセット =====
    auto* modelRenderer = scene_->GetComponent<ModelMeshRenderer>(playerEntityHandle_);
    if (modelRenderer) {
        for (auto& mesh : modelRenderer->GetAllTransformBuffRef()) {
            mesh.openData_.translate -= isRightWall_ ? -wallNormal_ * kMeshOffsetRate : wallNormal_ * kMeshOffsetRate;
        }
    }
}

void PlayerWallRunState::Update(float _deltaTime) {
    auto* state     = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* transform = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 衝突が途切れないようにめり込ませる
    transform->translate -= wallNormal_ * kOffsetRate;

    // 壁との衝突判定の残り時間を更新
    // これが0以下になると 壁から離れた と判定される
    if (state->IsCollisionWithWall()) {
        separationLeftTime_ = separationGraceTime_;
    } else {
        separationLeftTime_ -= _deltaTime;
    }

    // RumpUp 処理
    speedRumpUpTimer_ += _deltaTime;
    float rumpUpT          = speedRumpUpTimer_ / speedRumpUpTime_;
    rumpUpT                = std::clamp(rumpUpT, 0.f, 1.f);
    float currentSpeedRate = std::lerp(1.f, speedRate_, EaseOutCubic(rumpUpT));

    // 速度を更新
    auto* rigidbody          = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    OriGine::Vec3f direction = rigidbody->GetVelocity().normalize();
    OriGine::Vec3f newVelo   = direction * (playerSpeed_ * currentSpeedRate);
    rigidbody->SetVelocity(newVelo);

    gravityApplyDelay_ -= _deltaTime;
    if (gravityApplyDelay_ <= 0.0f) {
        rigidbody->SetUseGravity(true);
        gravityApplyDelay_ = 0.f;
    }
}

void PlayerWallRunState::Finalize() {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    rigidbody->SetMass(playerStatus->GetDefaultMass());
    rigidbody->SetUseGravity(true);
    playerStatus->SetupWallRunInterval();

    transform->translate += wallNormal_ * kOffsetRate;

    auto* modelRenderer = scene_->GetComponent<ModelMeshRenderer>(playerEntityHandle_);
    if (modelRenderer) {
        for (auto& mesh : modelRenderer->GetAllTransformBuffRef()) {
            mesh.openData_.translate = Vec3f();
        }
    }

    scene_->AddDeleteEntity(pathEntityHandle_);
}

PlayerMoveState PlayerWallRunState::TransitionState() const {
    if (separationLeftTime_ <= 0.0f) {
        return PlayerMoveState::FALL_DOWN;
    }

    auto playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    if (playerInput->IsWallJumpInput()) {
        return PlayerMoveState::WALL_JUMP;
    }

    auto* rigidbody = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    if (rigidbody->GetVelocity()[Y] < wallRunDetachSpeed_) {
        return PlayerMoveState::FALL_DOWN;
    }

    return PlayerMoveState::WALL_RUN;
}
