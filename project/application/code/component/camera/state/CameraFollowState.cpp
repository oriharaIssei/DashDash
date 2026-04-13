#include "CameraFollowState.h"

/// engine
#include "Engine.h"

/// component
#include "component/camera/CameraController.h"
#include "component/player/state/PlayerState.h"
#include "component/transform/CameraTransform.h"
#include "component/transform/Transform.h"

/// math
#include "MyEasing.h"
#include <cmath>

using namespace OriGine;

void CameraFollowState::Initialize() {
    lerpTimer_ = 0.0f;
}

void CameraFollowState::Update() {
    CameraController* cc = scene_->GetComponent<CameraController>(cameraEntityHandle_);
    if (!cc) {
        return;
    }

    PlayerState* state = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    if (!state) {
        return;
    }

    float t = 0.f;
    if (state->GetGearLevel() >= kThresholdGearLevel_) {
        float cameraDeltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Camera");
        lerpTimer_ += cameraDeltaTime;

        t = std::clamp(lerpTimer_ / kLerpTime_, 0.f, 1.f);
    }

    // ギアレベルに応じて目標パラメータを決定
    const CameraStateParams& target =
        (state->GetGearLevel() >= kThresholdGearLevel_) ? cc->dashParams : cc->idleParams;

    cc->currentOffset       = Lerp<3, float>(cc->currentOffset, target.offset, EaseOutCubic(t));
    cc->currentTargetOffset = Lerp<3, float>(cc->currentTargetOffset, target.targetOffset, EaseOutCubic(t));

    // ターゲットの左右ズレに応じたZ回転(ロール)
    auto* targetTransform = scene_->GetComponent<Transform>(cc->followTargetEntity);
    auto* cameraTransform = scene_->GetComponent<CameraTransform>(cameraEntityHandle_);
    if (targetTransform && cameraTransform) {
        Vec3f toTarget = Vec3f::Normalize(targetTransform->GetWorldTranslate() - cameraTransform->translate);
        Vec3f forward  = cc->baseRotate.RotateVector(axisZ);
        Vec3f right    = cc->baseRotate.RotateVector(axisX);

        // カメラ正面とターゲット方向の内積で前方にいるか判定
        float forwardDot = Vec3f::Dot(forward, toTarget);

        if (forwardDot >= cc->tiltDotOnFollow) {
            // right方向への射影で左右のズレ量を取得 (-1 ~ 1)
            float rightDot     = std::clamp(Vec3f::Dot(right, toTarget), -1.f, 1.f);
            cc->currentRotateZ = -rightDot * cc->maxRollAngleOnFollow;
        } else {
            cc->currentRotateZ = 0.f;
        }
    }
}

void CameraFollowState::Finalize() {}
