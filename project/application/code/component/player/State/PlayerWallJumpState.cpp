#include "PlayerWallJumpState.h"

/// component
#include "component/animation/TransformAnimation.h"
#include "component/transform/CameraTransform.h"
#include "component/transform/Transform.h"

#include "component/camera/CameraController.h"
#include "component/physics/Rigidbody.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerMoveUtils.h"
#include "component/player/PlayerStatus.h"

/// log
#include "logger/Logger.h"
/// math
#include "MyEasing.h"

using namespace OriGine;

void PlayerWallJumpState::Initialize() {
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* playerState  = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);

    rigidbody->SetAcceleration({0.0f, 0.0f, 0.0f});
    rigidbody->SetUseGravity(false);

    // 壁情報
    const OriGine::Vec3f& wallNormal = playerState->GetWallCollisionNormal().normalize();
    OriGine::Vec3f jumpDirWorld      = Vec3f();
    float jumpSpeed                  = 0.f;

    // プレイヤーの進行方向（正面）
    OriGine::Vec3f velocityDirection = rigidbody->GetVelocity();
    velocityDirection                = velocityDirection.normalize();

    OriGine::Vec3f wallJumpDirection = playerStatus->GetWallJumpOffset();
    // -1 ~ 1 を 0 ~ 1 に変換
    float inputXNormalized = (playerInput->GetInputDirection()[X] + 1) * 0.5f;
    inputXNormalized       = EasingFunctions[static_cast<int>(EaseType::EaseInQuad)](inputXNormalized);

    wallJumpDirection[X] = std::lerp(playerStatus->GetMinWallJumpOffsetX(), wallJumpDirection[X], inputXNormalized);

    // --- 壁ローカル → ワールド変換 ---
    // wallJumpDirection = (x:外, y:上, z:沿う)
    jumpDirWorld =
        wallNormal * wallJumpDirection[X] + axisY * wallJumpDirection[Y] + velocityDirection * wallJumpDirection[Z];
    // wallRun後は maxXZSpeedを使用する
    jumpSpeed = playerStatus->GetCurrentMaxSpeed() * playerStatus->GetWallRunRate();

    jumpDirWorld = jumpDirWorld.normalize();

    // --- 最終速度設定 ---
    velo_ = jumpDirWorld * jumpSpeed;

    rigidbody->SetVelocity(velo_);

    // animation
    TransformAnimation* transformAnimation = scene_->GetComponent<TransformAnimation>(playerEntityHandle_);
    if (transformAnimation) {
        OriGine::Vec3f sideJudgeDirection = velocityDirection;
        sideJudgeDirection[Y]             = 0.0f;
        if (sideJudgeDirection.lengthSq() <= 0.0f) {
            sideJudgeDirection = axisZ;
        }

        const bool isRightWall = PlayerMoveUtils::IsWallRight(sideJudgeDirection.normalize(), wallNormal);

        auto rotateFlip = transformAnimation->GetRotateFlip();
        auto scaleFlip  = transformAnimation->GetScaleFlip();

        rotateFlip.z = isRightWall;

        transformAnimation->SetRotateFlip(rotateFlip);
        transformAnimation->SetScaleFlip(scaleFlip);

        transformAnimation->PlayStart();
    }

    forceJumpTimer_ = 0.f;
}

void PlayerWallJumpState::Update(float _deltaTime) {
    forceJumpTimer_ += _deltaTime;
}

void PlayerWallJumpState::Finalize() {
    auto* rigidbody = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    rigidbody->SetUseGravity(true); // 重力を有効

    forceJumpTimer_ = kForceJumpTime_; // 強制時間を終了させる
}

PlayerMoveState PlayerWallJumpState::TransitionState() const {
    auto* playerState = scene_->GetComponent<PlayerState>(playerEntityHandle_);

    if (playerState->IsOnGround()) {
        return PlayerMoveState::DASH;
    }
    // Rail上にいる場合
    if (playerState->IsOnRail()) {
        return PlayerMoveState::RUN_ON_RAIL;
    } else if (playerState->IsCollisionWithWall()) {
        return PlayerMoveState::WALL_RUN;
    }

    // 強制時間が終了したら 落下状態へ
    if (forceJumpTimer_ >= kForceJumpTime_) {
        return PlayerMoveState::FALL_DOWN;
    }

    return PlayerMoveState::WALL_JUMP;
}
