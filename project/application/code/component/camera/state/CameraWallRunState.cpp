#include "CameraWallRunState.h"

/// engine
#include "Engine.h"

/// component
#include "component/camera/CameraController.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerMoveUtils.h"
#include "component/player/state/PlayerState.h"
#include "component/physics/Rigidbody.h"

/// math
#include "Interpolation.h"
#include "MyEasing.h"
#include "math/MathEnv.h"

using namespace OriGine;

void CameraWallRunState::Initialize() {
    CameraController* cc = scene_->GetComponent<CameraController>(cameraEntityHandle_);
    PlayerState* state   = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    Rigidbody* rigidbody = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    if (!cc || !state || !rigidbody) {
        return;
    }

    Vec3f wallNormal = state->GetWallCollisionNormal().normalize();
    Vec3f direction  = PlayerMoveUtils::ComputeWallRunDirection(rigidbody->GetVelocity(), wallNormal);
    direction[Y]     = 0.0f;
    if (direction.lengthSq() > kEpsilon) {
        direction = direction.normalize();
    }

    isRightWall_ = PlayerMoveUtils::IsWallRight(direction, wallNormal);

    // 左壁想定のオフセットを取得
    targetOffsetOnWallRun_     = cc->wallRunParams.targetOffset;
    minTargetOffsetXOnWallRun_ = cc->minTargetOffsetXOnWallRun;
    offsetOnWallRun_           = cc->wallRunParams.offset;

    // 右壁なら左右反転
    if (!isRightWall_) {
        targetOffsetOnWallRun_[X] *= -1.0f;
        minTargetOffsetXOnWallRun_ *= -1.0f;
        offsetOnWallRun_[X] *= -1.0f;
    }

    lerpTimer_ = 0.0f;
}

void CameraWallRunState::Update() {
    CameraController* cc = scene_->GetComponent<CameraController>(cameraEntityHandle_);
    if (!cc) {
        return;
    }

    float cameraDeltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Camera");
    lerpTimer_ += cameraDeltaTime;
    float t = lerpTimer_ / kLerpTime_;

    if (t <= 1.0f) {
        cc->currentOffset       = Lerp<3, float>(cc->currentOffset, offsetOnWallRun_, EaseOutCubic(t));
        cc->currentTargetOffset = Lerp<3, float>(cc->currentTargetOffset, targetOffsetOnWallRun_, EaseOutCubic(t));
    } else {
        cc->currentOffset       = offsetOnWallRun_;
        cc->currentTargetOffset = targetOffsetOnWallRun_;
    }

    // 移動方向に応じて、カメラのオフセットのX成分の目標値を変える
    PlayerInput* playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    if (playerInput) {
        float inputXNormalized = (playerInput->GetInputDirection()[X] + 1) * 0.5f; // [-1, 1] -> [0, 1]
        if (isRightWall_) {
            inputXNormalized = 1 - inputXNormalized; // 右壁なら反転
        }
        inputXNormalized = EasingFunctions[static_cast<int>(EaseType::EaseInCubic)](inputXNormalized);

        cc->currentTargetOffset[X] = std::lerp(minTargetOffsetXOnWallRun_, targetOffsetOnWallRun_[X], inputXNormalized);
    }
}

void CameraWallRunState::Finalize() {
    CameraController* cc = scene_->GetComponent<CameraController>(cameraEntityHandle_);
    if (cc) {
        cc->currentOffset       = cc->wallRunParams.offset;
        cc->currentTargetOffset = cc->dashParams.targetOffset;
    }
}
