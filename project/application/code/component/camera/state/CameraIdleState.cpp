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
    float t = std::clamp(lerpTimer_ / kLerpTime_, 0.f, 1.f);

    cc->currentOffset       = Lerp<3, float>(cc->currentOffset, cc->idleParams.offset, EaseOutCubic(t));
    cc->currentTargetOffset = Lerp<3, float>(cc->currentTargetOffset, cc->idleParams.targetOffset, EaseOutCubic(t));
}

void CameraIdleState::Finalize() {}
