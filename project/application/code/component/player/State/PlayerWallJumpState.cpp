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

    // 壁ジャンプは踏み切りの瞬間に決めた速度をそのまま維持したいので、
    // 加速度を打ち消し重力も切る。放物線にせず直線的に飛ばすことで、
    // プレイヤーが着地点を予測しやすい挙動にしている。
    // 重力はFinalize()で必ず戻すこと(戻し忘れると以降ずっと浮いたままになる)
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
    // 壁法線(外向き)・ワールド上方向・進行方向の3ベクトルを基底として、
    // 壁基準で指定されたジャンプ方向をワールド座標へ組み立てる。
    // 壁の向きに関わらず「外へ・上へ・壁に沿って」の配合比を同じ操作感で扱えるようにするため
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
        // 左右どちらの壁を蹴ったかでモデルの回転を反転させる。
        // 判定に使うのは水平方向の向きだけなのでY成分を落とすが、真上/真下に飛んでいる場合は
        // ゼロベクトルになりnormalizeでNaNが出るため、既定の前方向(axisZ)で代用する
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

    // 上の接地/レール/壁判定より後に置いているのは、この状態を「最低限kForceJumpTime_だけは維持する」
    // ためのフォールバックだから。踏み切り直後はまだ壁のすぐ近くにいて壁判定が残りやすく、
    // 先に時間切れを見てしまうと壁ジャンプが即座に打ち切られて飛距離が出ない
    // 強制時間が終了したら 落下状態へ
    if (forceJumpTimer_ >= kForceJumpTime_) {
        return PlayerMoveState::FALL_DOWN;
    }

    return PlayerMoveState::WALL_JUMP;
}
