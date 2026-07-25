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

    // ギアが基準未満の間はタイマーを進めずt=0のままにする。
    // Lerpの係数が0なら現在値が保持されるため、加速するまでカメラは引きの位置に留まり、
    // 基準ギアに達した瞬間から寄りの構図へ動き出す
    float t = 0.f;
    if (state->GetGearLevel() >= kThresholdGearLevel_) {
        float cameraDeltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Camera");
        lerpTimer_ += cameraDeltaTime;

        t = std::clamp(lerpTimer_ / kLerpTime_, 0.f, 1.f);
    }

    // ギアレベルに応じて目標パラメータを決定
    const CameraStateParams& target =
        (state->GetGearLevel() >= kThresholdGearLevel_) ? cc->GetDashParams() : cc->GetIdleParams();

    cc->SetCurrentOffset(Lerp<3, float>(cc->GetCurrentOffset(), target.offset, EaseOutCubic(t)));
    cc->SetCurrentTargetOffset(Lerp<3, float>(cc->GetCurrentTargetOffset(), target.targetOffset, EaseOutCubic(t)));

    // ターゲットの左右ズレに応じたZ回転(ロール)
    auto* targetTransform = scene_->GetComponent<Transform>(cc->GetFollowTargetEntity());
    auto* cameraTransform = scene_->GetComponent<CameraTransform>(cameraEntityHandle_);
    if (targetTransform && cameraTransform) {
        Vec3f toTarget = Vec3f::Normalize(targetTransform->GetWorldTranslate() - cameraTransform->translate);
        Vec3f forward  = cc->GetBaseRotate().RotateVector(axisZ);
        Vec3f right    = cc->GetBaseRotate().RotateVector(axisX);

        // カメラ正面とターゲット方向の内積で前方にいるか判定
        float forwardDot = Vec3f::Dot(forward, toTarget);

        // ターゲットが画面の外や背後にいる状態でロールを掛けると、
        // 画面が大きく傾くだけで何が起きているか分からなくなる。
        // 内積が閾値以上＝十分に正面にいるときだけ演出を有効にする
        if (forwardDot >= cc->GetTiltDotOnFollow()) {
            // right方向への射影で左右のズレ量を取得 (-1 ~ 1)
            // 符号を反転しているのは、ターゲットが右にいるとき画面を左に傾けるため。
            // 曲がる方向の内側に傾くことで、バイクがカーブでバンクする挙動を再現している
            float rightDot = std::clamp(Vec3f::Dot(right, toTarget), -1.f, 1.f);
            cc->SetCurrentRotateZ(-rightDot * cc->GetMaxRollAngleOnFollow());
        } else {
            cc->SetCurrentRotateZ(0.f);
        }
    }
}

void CameraFollowState::Finalize() {}
