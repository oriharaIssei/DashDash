#include "PlayerJumpState.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerMoveUtils.h"
#include "component/player/PlayerStatus.h"
#include "component/player/State/PlayerState.h"
#include "component/transform/CameraTransform.h"
#include "component/transform/Transform.h"

///  math
#include "math/MyEasing.h"

using namespace OriGine;

void PlayerJumpState::Initialize() {
    releaseJumpPower_ = 0.f;

    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* playerState  = scene_->GetComponent<PlayerState>(playerEntityHandle_);

    rigidbody->SetUseGravity(false);

    float t      = static_cast<float>(playerState->GetGearLevel()) / static_cast<float>(kMaxPlayerGearLevel);
    float easedT = EasingFunctions[static_cast<int32_t>(playerStatus->GetJumpHoldVelocityEaseType())](t);

    rigidbody->SetVelocity(Y, std::lerp(playerStatus->GetMinJumpHoldVelocity(), playerStatus->GetMaxJumpHoldVelocity(), t)); // ジャンプパワーをY軸に設定

    easedT       = EasingFunctions[static_cast<int32_t>(playerStatus->GetJumpChargeRateEaseType())](t);
    chargePower_ = std::lerp(playerStatus->GetMinJumpChargeRate(), playerStatus->GetMaxJumpChargeRate(), easedT);

    maxChargePower_ = chargePower_ * playerInput->GetMaxJumpTime(); // ジャンプ力の最大値を計算
}

void PlayerJumpState::Update(float _deltaTime) {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* state        = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 移動処理
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

    // ジャンプ力の蓄積
    releaseJumpPower_ += chargePower_ * _deltaTime;
}

void PlayerJumpState::Finalize() {
    auto* rigidbody = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    rigidbody->SetUseGravity(true);

    // 蓄えたジャンプ力を適用
    rigidbody->SetVelocity(Y, rigidbody->GetVelocity()[Y] + std::clamp(releaseJumpPower_, 0.f, maxChargePower_));
}

PlayerMoveState PlayerJumpState::TransitionState() const {
    auto state       = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* rigidbody  = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    // Rail上にいる場合
    if (state->IsOnRail()) {
        return PlayerMoveState::RUN_ON_RAIL;
    }

    if (state->IsCollisionWithWall()) {
        return PlayerMoveState::WALL_RUN;
    }

    if (state->IsOnGround()) {
        if (playerInput->GetInputDirection().lengthSq() > 0.f) {
            return PlayerMoveState::DASH;
        }
        return PlayerMoveState::IDLE;
    } else {
        if (!playerInput->IsJumpInput() || releaseJumpPower_ >= maxChargePower_ || rigidbody->GetVelocity(Y) >= maxChargePower_) {
            // ジャンプ入力がない場合は落下状態に遷移
            return PlayerMoveState::FALL_DOWN;
        }
    }

    return PlayerMoveState::JUMP;
}
