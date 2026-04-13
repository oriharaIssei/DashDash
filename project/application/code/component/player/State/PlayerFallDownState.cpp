#include "PlayerFallDownState.h"

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

using namespace OriGine;

void PlayerFallDownState::Initialize() {}

void PlayerFallDownState::Update(float _deltaTime) {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* playerState  = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 速度更新
    CameraTransform* cameraTransform = scene_->GetComponent<CameraTransform>(playerState->GetCameraEntityHandle());
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

}

void PlayerFallDownState::Finalize() {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    rigidbody->SetAcceleration({0.0f, 0.0f, 0.0f}); // 加速度をリセット
    OriGine::Vec3f velo = rigidbody->GetVelocity();

    float limitSpeed = playerStatus->GetCurrentMaxSpeed();
    if (velo.lengthSq() >= limitSpeed * limitSpeed) {
        // 速度が速すぎる場合は制限
        velo = velo.normalize() * limitSpeed;
        rigidbody->SetVelocity(velo);
    }
}

PlayerMoveState PlayerFallDownState::TransitionState() const {
    auto state       = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);

    // Rail上にいる場合
    if (state->IsOnRail()) {
        return PlayerMoveState::RUN_ON_RAIL;
    }
    // 壁走り判定
    if (state->IsCollisionWithWall()) {
        return PlayerMoveState::WALL_RUN;
    }

    // 着地判定
    if (state->IsOnGround()) {
        if (playerInput->GetInputDirection().lengthSq() > 0.f) {
            return PlayerMoveState::DASH;
        }
        return PlayerMoveState::IDLE;
    }

    return PlayerMoveState::FALL_DOWN;
}
