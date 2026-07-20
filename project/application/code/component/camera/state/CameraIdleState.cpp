#include "CameraIdleState.h"

/// engine
#include "Engine.h"

/// component
#include "component/camera/CameraController.h"

/// math
#include "MyEasing.h"

using namespace OriGine;

void CameraIdleState::Initialize() {
    lerpTimer_ = 0.0f;
}

void CameraIdleState::Update() {
    CameraController* cc = scene_->GetComponent<CameraController>(cameraEntityHandle_);
    if (!cc) {
        return;
    }

    float cameraDeltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Camera");
    lerpTimer_ += cameraDeltaTime;
    // 経過時間をkLerpTime_で正規化し、0〜1の補間係数を求める
    float t = std::clamp(lerpTimer_ / kLerpTime_, 0.f, 1.f);

    // 現在のオフセット/注視点オフセットをidleParamsへEaseOutCubicで補間する
    cc->SetCurrentOffset(Lerp<3, float>(cc->GetCurrentOffset(), cc->GetIdleParams().offset, EaseOutCubic(t)));
    cc->SetCurrentTargetOffset(Lerp<3, float>(cc->GetCurrentTargetOffset(), cc->GetIdleParams().targetOffset, EaseOutCubic(t)));
}

void CameraIdleState::Finalize() {}
