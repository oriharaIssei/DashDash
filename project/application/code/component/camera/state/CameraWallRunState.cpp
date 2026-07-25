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

    // 壁走り中にカメラを壁のどちら側に出すかを決めるため、進行方向が壁の左右どちらを向いているかを調べる。
    // 判定に必要なのは水平面上の向きだけなので、Y成分を落として真上から見た方向に潰してから正規化する。
    // (Y成分を残すと坂の登り下りで左右判定が揺れ、カメラが壁をまたいで反転してしまう)
    Vec3f wallNormal = state->GetWallCollisionNormal().normalize();
    Vec3f direction  = PlayerMoveUtils::ComputeWallRunDirection(rigidbody->GetVelocity(), wallNormal);
    direction[Y]     = 0.0f;
    // 壁に対してほぼ真上/真下に進んでいる場合、Y成分を落とすとゼロベクトルになる。
    // そのままnormalizeするとゼロ除算でNaNになるため、長さがある場合のみ正規化する
    if (direction.lengthSq() > kEpsilon) {
        direction = direction.normalize();
    }

    isRightWall_ = PlayerMoveUtils::IsWallRight(direction, wallNormal);

    // 左壁想定のオフセットを取得
    targetOffsetOnWallRun_     = cc->GetWallRunParams().targetOffset;
    minTargetOffsetXOnWallRun_ = cc->GetMinTargetOffsetXOnWallRun();
    offsetOnWallRun_           = cc->GetWallRunParams().offset;

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

    // 補間の始点に固定値ではなく「現在のオフセット」を渡しているため、
    // 前ステートのオフセットがどこであってもそこから連続して繋がる(カメラが飛ばない)。
    // ただし毎フレーム始点が動くので、これは等速の補間ではなく目標値へ収束する平滑化になる。
    // t>1.0 では誤差が残ったままになるので、下のelseで目標値を直接代入して確実に一致させている
    if (t <= 1.0f) {
        cc->SetCurrentOffset(Lerp<3, float>(cc->GetCurrentOffset(), offsetOnWallRun_, EaseOutCubic(t)));
        cc->SetCurrentTargetOffset(Lerp<3, float>(cc->GetCurrentTargetOffset(), targetOffsetOnWallRun_, EaseOutCubic(t)));
    } else {
        cc->SetCurrentOffset(offsetOnWallRun_);
        cc->SetCurrentTargetOffset(targetOffsetOnWallRun_);
    }

    // 移動方向に応じて、カメラのオフセットのX成分の目標値を変える
    PlayerInput* playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    if (playerInput) {
        float inputXNormalized = (playerInput->GetInputDirection()[X] + 1) * 0.5f; // [-1, 1] -> [0, 1]
        if (isRightWall_) {
            inputXNormalized = 1 - inputXNormalized; // 右壁なら反転
        }
        // EaseInCubic(3乗)を掛けることで入力の小さい範囲での変化を鈍らせる。
        // 線形のままだとスティックの僅かな揺れがそのままカメラの横揺れになって画面が酔うため
        inputXNormalized = EasingFunctions[static_cast<int>(EaseType::EaseInCubic)](inputXNormalized);

        Vec3f currentTargetOffset    = cc->GetCurrentTargetOffset();
        currentTargetOffset[X]       = std::lerp(minTargetOffsetXOnWallRun_, targetOffsetOnWallRun_[X], inputXNormalized);
        cc->SetCurrentTargetOffset(currentTargetOffset);
    }
}

/// <summary>
/// 壁走りステートを抜ける際に、カメラのオフセットを次ステートの基準値へ戻す。
/// Initializeで左右反転させた値が残ったままだと、壁から離れた後もカメラが横にずれ続けるため、
/// 反転前のパラメータを読み直して打ち消している。
/// </summary>
void CameraWallRunState::Finalize() {
    CameraController* cc = scene_->GetComponent<CameraController>(cameraEntityHandle_);
    if (cc) {
        cc->SetCurrentOffset(cc->GetWallRunParams().offset);
        cc->SetCurrentTargetOffset(cc->GetDashParams().targetOffset);
    }
}
