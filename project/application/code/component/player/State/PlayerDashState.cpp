#include "PlayerDashState.h"

/// engine
#include "Engine.h"

/// component
#include "component/transform/CameraTransform.h"
#include "component/transform/Transform.h"

#include "component/physics/Rigidbody.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerMoveUtils.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

/// math
#include "MyEasing.h"
#include "SpringDamper.h"
#include <MathEnv.h>

using namespace OriGine;

void PlayerDashState::Initialize() {
    auto* state        = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 速度を初期化
    int32_t gearLevel     = state->GetGearLevel();
    float currentMaxSpeed = playerStatus->CalculateSpeedByGearLevel(gearLevel);
    playerStatus->SetCurrentMaxSpeed(currentMaxSpeed);
    playerStatus->SetCurrentMaxDirectionalSpeed(playerStatus->CalculateCurrentMaxDirectionalSpeed(gearLevel));
    playerStatus->SetCurrentDirectionalSpeed({0.f, 0.f});
    // プレイヤーの向いている方向に速度を設定
    Vec3f forwardDir = transform->FrontVector();
    forwardDir[Y]    = 0.f;
    forwardDir       = forwardDir.normalize();
    rigidbody->SetVelocity(forwardDir * currentMaxSpeed);

}

void PlayerDashState::Update(float _deltaTime) {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* state        = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    /// ---------------------------------------------------------------------
    // gearLevel の更新 (走っている間だけ)
    playerStatus->minusGearUpCoolTime(_deltaTime);

    int32_t gearLevel = state->GetGearLevel();
    // ギアレベルが最大に達していない場合、
    if (gearLevel < kMaxPlayerGearLevel - 1) {
        // クールタイムが0以下になったらギアレベルを上げる
        if (playerStatus->GetGearUpCoolTime() <= 0.f) {
            auto& stateFlag = state->GetStateFlagRef();
            stateFlag.SetCurrent(stateFlag.Current() | PlayerStateFlag::GEAR_UP);

            ++gearLevel;
            state->SetGearLevel(gearLevel);

            playerStatus->SetGearUpCoolTime(playerStatus->CalculateCoolTimeByGearLevel(gearLevel));

            playerStatus->SetupOnGearUp(gearLevel);
        }
    }

    // 速度更新
    CameraTransform* cameraTransform = scene_->GetComponent<CameraTransform>(state->GetCameraEntityHandle());
    if (cameraTransform) {
        Vec3f worldInputDir    = playerInput->CalculateWorldInputDirection(cameraTransform->rotate);
        Vec3f forwardDirection = playerStatus->ComputeSmoothedDirection(worldInputDir, rigidbody, transform, _deltaTime);

        rigidbody->SetVelocity(
            PlayerMoveUtils::UpdatePlanarVelocity(playerStatus, rigidbody->GetVelocity(), playerInput->GetInputDirection()[X], forwardDirection, _deltaTime));

        // プレイヤーの向きを速度方向に回転させる
        Vec3f dir = rigidbody->GetVelocity();
        if (Vec2f(dir[X], dir[Z]).lengthSq() > 0.f) {
            dir[Y]            = 0.f;
            dir               = dir.normalize();
            transform->rotate = Quaternion::LookAt(dir, axisY);
        }
    }

    // 落下時間を更新
    if (state->IsOnGround()) {
        fallDownTimer_ = kFallDownThresholdTime_;
    } else {
        fallDownTimer_ -= _deltaTime;
    }

}

void PlayerDashState::Finalize() {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    OriGine::Vec3f velo = rigidbody->GetVelocity();

    float limitSpeed = playerStatus->GetCurrentMaxSpeed(); // このフレームに追加される速度を引く
    if (velo.lengthSq() >= limitSpeed * limitSpeed) {
        // 速度が速すぎる場合は制限
        velo = velo.normalize() * limitSpeed;
        rigidbody->SetVelocity(velo);
    }
}

PlayerMoveState PlayerDashState::TransitionState() const {
    auto state       = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto rigidbody   = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    // 入力がない場合はアイドル状態に遷移
    if (playerInput->GetInputDirection().lengthSq() <= 0.f) {
        return PlayerMoveState::IDLE;
    }

    // 上昇速度がある場合は落下状態に遷移
    if (rigidbody->GetVelocity()[Y] > kEpsilon) {
        return PlayerMoveState::FALL_DOWN;
    }

    // Rail上にいる場合
    if (state->IsOnRail()) {
        return PlayerMoveState::RUN_ON_RAIL;
    }
    // 地面にいる場合
    if (state->IsOnGround()) {
        if (playerInput->IsJumpInput()) {
            return PlayerMoveState::JUMP;
        }
    } else {
        // 空中にいる場合は落下状態に遷移
        // 一定時間経過したら落下状態に遷移
        if (fallDownTimer_ <= 0.f) {
            return PlayerMoveState::FALL_DOWN;
        }
    }

    return PlayerMoveState::DASH;
}
