#include "PlayerWallRunState.h"

/// engine
#include "scene/SceneFactory.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "component/renderer/ModelMeshRenderer.h"
#include "component/spline/SplinePoints.h"
#include "component/transform/Transform.h"

#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

/// log
#include "logger/Logger.h"

/// util
#include "component/player/PlayerConfig.h"
#include "component/player/PlayerMoveUtils.h"
#include "util/globalVariables/SerializedField.h"

/// math
#include "Interpolation.h"
#include "math/MathEnv.h"
#include "MyEasing.h"

using namespace OriGine;

namespace {
constexpr float kOffsetRate = AppConfig::Player::kWallRunOffsetRate;
}

void PlayerWallRunState::Initialize() {
    constexpr int32_t thresholdGearLevel = AppConfig::Player::kWallRunThresholdGearLevel;
    constexpr float kMeshOffsetRate      = AppConfig::Player::kWallRunMeshOffsetRate;

    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* state        = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 壁ジャンプ前の速度
    prevVelo_ = rigidbody->GetVelocity();

    // 壁法線
    wallNormal_ = state->GetWallCollisionNormal().normalize();

    // ===== 進行方向を「速度を壁面に投影して」求める =====
    OriGine::Vec3f direction = PlayerMoveUtils::ComputeWallRunDirection(prevVelo_, wallNormal_);
    direction[Y]             = 0.0f;

    // Y成分を落とすのは、壁走り中に上下方向へ向きが振れて機体が傾くのを防ぐため(水平方向の向きだけ使う)
    if (direction.lengthSq() > kEpsilon) {
        direction = direction.normalize();
    } else {
        // フォールバック（ほぼ起きない）
        // 壁に真正面から当たった場合など、投影後の水平成分が消えてゼロベクトルになると
        // normalizeでNaNが出て以降の速度計算が全て壊れる。その場合は現在の機体の向きを進行方向にする
        direction = axisZ * MakeMatrix4x4::RotateY(transform->rotate.ToYaw());
    }

    // ===== スピード =====
    playerSpeed_ = playerStatus->CalculateSpeedByGearLevel(state->GetGearLevel());

    // 基準レベル未満ならギアアップ
    // 壁走りは低速だと壁に貼り付けず落下してしまうため、最低限の速度を保証する目的で
    // 進入時に1段だけギアを上げる。上限(thresholdGearLevel)を設けているのは、
    // 壁走りを繰り返すだけで際限なく加速できてしまうのを防ぐため
    int32_t gearLevel = state->GetGearLevel();
    if (gearLevel < thresholdGearLevel) {
        auto& stateFlag = state->GetStateFlagRef();
        stateFlag.SetCurrent(stateFlag.Current() | PlayerStateFlag::GEAR_UP);

        int32_t addedGearLevel = gearLevel + 1;
        state->SetGearLevel(addedGearLevel);

        playerStatus->SetGearUpCoolTime(
            playerStatus->CalculateCoolTimeByGearLevel(addedGearLevel));

        playerStatus->SetCurrentMaxSpeed(
            playerStatus->CalculateSpeedByGearLevel(addedGearLevel));

        playerSpeed_ = playerStatus->GetCurrentMaxSpeed();
    }

    // ===== 移動 =====
    rigidbody->SetVelocity(playerSpeed_ * direction);

    rigidbody->SetMass(playerStatus->GetMassOnWallRun());

    // 壁との分離猶予
    separationLeftTime_ = separationGraceTime_;
    // 壁走り離脱速度
    wallRunDetachSpeed_ = playerStatus->GetWallRunDetachSpeed();

    // ===== 向きとロール =====
    PlayerEffectControlParam* effectParam = scene_->GetComponent<PlayerEffectControlParam>(playerEntityHandle_);
    isRightWall_                          = PlayerMoveUtils::IsWallRight(direction, wallNormal_);

    float rotateZOffsetOnWallRun = effectParam->GetRotateOffsetOnWallRun();
    // プレイヤーの向きを移動方向に合わせる
    Quaternion lookForward = Quaternion::LookAt(direction, axisY);
    // 回転アニメーションのゴール地点を設定
    Quaternion angleOffset = Quaternion::RotateAxisAngle(axisZ, isRightWall_ ? rotateZOffsetOnWallRun : -rotateZOffsetOnWallRun);
    transform->rotate      = lookForward * angleOffset;

    // ===== スピード制御 =====
    speedRate_        = playerStatus->GetWallRunRate();
    speedRumpUpTime_  = playerStatus->GetWallRunRampUpTime();
    speedRumpUpTimer_ = 0.0f;

    // ===== 重力制御 =====
    // 進入直後に重力を掛けると壁を掴む前に落ち始めてしまうため、一定時間だけ重力を切る。
    // この猶予がプレイヤーにとっての「壁走りできる時間」の実体になっている。
    // Update()でdelayを減算し0になった時点で重力を戻す
    gravityApplyDelay_ = playerStatus->GetGravityApplyDelayOnWallRun();
    rigidbody->SetUseGravity(false);

    // ===== メッシュのオフセット =====
    // 当たり判定の中心は壁にめり込ませたままにする一方、見た目のメッシュだけを壁側へ寄せて
    // 「壁に張り付いて走っている」画にする。壁が右か左かで寄せる向きが逆になるため符号を反転する。
    // (減算と-wallNormal_の二重否定で、右壁のときは法線方向＝壁から離れる向きに動く点に注意)
    // ここでずらしたオフセットはFinalize()でゼロに戻す
    auto* modelRenderer = scene_->GetComponent<ModelMeshRenderer>(playerEntityHandle_);
    if (modelRenderer) {
        for (auto& mesh : modelRenderer->GetAllTransformBuffRef()) {
            mesh.openData_.translate -= isRightWall_ ? -wallNormal_ * kMeshOffsetRate : wallNormal_ * kMeshOffsetRate;
        }
    }
}

void PlayerWallRunState::Update(float _deltaTime) {
    auto* state     = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* transform = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    // 衝突が途切れないようにめり込ませる
    // 衝突解決で毎フレーム壁の外へ押し戻されるため、押し戻された分をここで押し込み直す。
    // これを怠ると接触が1フレーム途切れた瞬間に壁判定が外れ、壁走りが不意に中断される。
    // 毎フレーム減算しているが位置は押し戻しと釣り合うので蓄積せず、
    // Finalize()で最後の1回分だけ戻せば元の位置に復帰する
    transform->translate -= wallNormal_ * kOffsetRate;

    // 壁との衝突判定の残り時間を更新
    // これが0以下になると 壁から離れた と判定される
    if (state->IsCollisionWithWall()) {
        separationLeftTime_ = separationGraceTime_;
    } else {
        separationLeftTime_ -= _deltaTime;
    }

    // RumpUp 処理
    // 進入直後に最高倍率を掛けると速度が跳ねて操作不能になるため、speedRate_まで時間を掛けて上げる。
    // 経過時間は上限なく増え続けるので、比率は0〜1にクランプしてから補間に渡す
    // (クランプしないとEaseOutCubicがt>1で1を超え、速度が青天井に伸びてしまう)
    speedRumpUpTimer_ += _deltaTime;
    float rumpUpT          = speedRumpUpTimer_ / speedRumpUpTime_;
    rumpUpT                = std::clamp(rumpUpT, 0.f, 1.f);
    float currentSpeedRate = std::lerp(1.f, speedRate_, EaseOutCubic(rumpUpT));

    // 速度を更新
    auto* rigidbody          = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    OriGine::Vec3f direction = rigidbody->GetVelocity().normalize();
    OriGine::Vec3f newVelo   = direction * (playerSpeed_ * currentSpeedRate);
    rigidbody->SetVelocity(newVelo);

    gravityApplyDelay_ -= _deltaTime;
    if (gravityApplyDelay_ <= 0.0f) {
        rigidbody->SetUseGravity(true);
        gravityApplyDelay_ = 0.f;
    }
}

void PlayerWallRunState::Finalize() {
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<OriGine::Transform>(playerEntityHandle_);

    rigidbody->SetMass(playerStatus->GetDefaultMass());
    rigidbody->SetUseGravity(true);
    playerStatus->SetupWallRunInterval();

    transform->translate += wallNormal_ * kOffsetRate;

    auto* modelRenderer = scene_->GetComponent<ModelMeshRenderer>(playerEntityHandle_);
    if (modelRenderer) {
        for (auto& mesh : modelRenderer->GetAllTransformBuffRef()) {
            mesh.openData_.translate = Vec3f();
        }
    }

    scene_->AddDeleteEntity(pathEntityHandle_);
}

/// <summary>
/// 壁走りから抜ける条件を判定する。
/// 壁から離れた・壁ジャンプ入力・落下速度超過の3つを、この優先順位で見る。
/// (壁から離れているのに壁ジャンプが成立してしまうと空中で無限にジャンプできるため、
///  分離判定を必ず先に評価する)
/// </summary>
PlayerMoveState PlayerWallRunState::TransitionState() const {
    // 壁との接触は凹凸や角で1フレーム単位で途切れることがある。即座に落下へ移ると
    // 挙動がガタつくため、separationGraceTime_の猶予を使い切って初めて離脱と判定する
    if (separationLeftTime_ <= 0.0f) {
        return PlayerMoveState::FALL_DOWN;
    }

    auto playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    if (playerInput->IsWallJumpInput()) {
        return PlayerMoveState::WALL_JUMP;
    }

    // 重力が効き始めて上昇の勢いが失われたら、壁に貼り付き続けず落下へ移す。
    // wallRunDetachSpeed_は既定値+5.0の「維持に必要な上昇速度の下限」であり、落下速度ではない点に注意。
    // つまりこの速度より速く上り続けている間だけ壁走りが継続し、失速した時点で離脱する
    auto* rigidbody = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    if (rigidbody->GetVelocity()[Y] < wallRunDetachSpeed_) {
        return PlayerMoveState::FALL_DOWN;
    }

    return PlayerMoveState::WALL_RUN;
}
