#include "PlayerOnRailState.h"

/// engine
#include "scene/Scene.h"

/// ECS
// component
#include "component/gimmick/RailPoints.h"
#include "component/physics/Rigidbody.h"
#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/State/PlayerState.h"
#include "component/transform/Transform.h"

/// math
#include "math/Interpolation.h"
#include "math/MathEnv.h"
#include "math/MyEasing.h"
#include "math/Spline.h"

using namespace OriGine;

namespace {
/// 入力方向を左/中央/右に3分割する閾値
constexpr float kInputDirThird = 1.0f / 3.0f;
} // namespace

PlayerOnRailState::PlayerOnRailState(OriGine::Scene* _scene, const OriGine::EntityHandle& _playerEntityHandle)
    : IPlayerMoveState(_scene, _playerEntityHandle, PlayerMoveState::RUN_ON_RAIL) {}
PlayerOnRailState::~PlayerOnRailState() {}

void PlayerOnRailState::Initialize() {
    auto* playerState  = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<Transform>(playerEntityHandle_);

    auto* railPoints    = scene_->GetComponent<RailPoints>(playerState->GetRailEntityHandle());
    auto* railTransform = scene_->GetComponent<Transform>(playerState->GetRailEntityHandle());

    // 移動は全て こちらで行うため、更新されないように速度情報を抜く
    rigidbody->SetIsActive(false);
    baseSpeed_               = playerStatus->GetCurrentMaxSpeed();
    defaultMaxFallDownSpeed_ = rigidbody->MaxFallSpeed();
    rigidbody->SetMaxFallSpeed(baseSpeed_ * playerStatus->GetRailSpeedRate());

    // 事前に必要な情報を計算する
    // レールの制御点はレールエンティティのローカル座標で保持されているため、
    // プレイヤーのワールド座標に逆行列を掛けて同じローカル空間へ持ち込んでから比較する。
    // (レール自体が移動・回転していても正しく最寄りのセグメントを求められるようにするため)
    Vec3f playerPosOnRail                           = transform->translate * railTransform->worldMat.inverse();
    std::pair<uint32_t, uint32_t> firstSegmentPoint = CalcPointSegmentIndex(railPoints->GetPoints(), playerPosOnRail);
    currentRailPointIndex_                          = firstSegmentPoint.first;
    nextRailPointIndex_                             = firstSegmentPoint.second;

    railTotalLength_ = CalcSplineLength(railPoints->GetPoints());

    // 正確な距離を計算：最も近いセグメントまでの累積距離 + セグメント内の射影距離
    float accumulatedLength = 0.0f;
    for (uint32_t i = 0; i < currentRailPointIndex_; ++i) {
        accumulatedLength += Vec3f(railPoints->GetPoints()[i + 1] - railPoints->GetPoints()[i]).length();
    }
    // 現在のセグメント内での射影位置を計算
    // 線分p1→p2上へ点を射影する式: t = (p-p1)・(p2-p1) / |p2-p1|^2
    // 内積を長さの2乗で割ることでtは「線分上の位置を0〜1で表した比率」になる。
    // clampしているのは、プレイヤーが線分の外側にいるとtが範囲外になり、
    // レール上に存在しない位置へワープしてしまうため。
    // 制御点が重複していると|p2-p1|^2が0になりゼロ除算するので、その場合は始点(t=0)へ落とす
    const Vec3f& p1    = railPoints->GetPoints()[currentRailPointIndex_];
    const Vec3f& p2    = railPoints->GetPoints()[nextRailPointIndex_];
    Vec3f lineDir      = p2 - p1;
    float lineLengthSq = lineDir.lengthSq();
    float t            = (lineLengthSq > 0.0f) ? std::clamp(Vec3f(playerPosOnRail - p1).dot(lineDir) / lineLengthSq, 0.0f, 1.0f) : 0.0f;
    traveledDistance_  = accumulatedLength + lineDir.length() * t;
}

void PlayerOnRailState::Update(float _deltaTime) {
    constexpr Vec3f kPlayerOffsetOnRail = Vec3f(0.f, 2.f, 0.f);
    auto* playerState                   = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto* playerInput                   = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* playerStatus                  = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* transform                     = scene_->GetComponent<Transform>(playerEntityHandle_);
    auto* rigidbody                     = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    auto* railPoints    = scene_->GetComponent<RailPoints>(playerState->GetRailEntityHandle());
    auto* railTransform = scene_->GetComponent<Transform>(playerState->GetRailEntityHandle());

    // railがなければ
    if (!railPoints) {
        isOutOfRail_ = false;
        return;
    }

    if (currentRailPointIndex_ > railPoints->GetPoints().size()) {
        isOutOfRail_ = false;
        return;
    }

    // 使用する一時変数
    const Vec3f& currentPoint = railPoints->GetPoints()[currentRailPointIndex_];
    const Vec3f& nextPoint    = railPoints->GetPoints()[nextRailPointIndex_];
    currentDirection_         = nextPoint - currentPoint;
    Vec3f front               = currentDirection_.normalize();

    // 速度
    rampUpTimer_ += _deltaTime;
    float t       = (std::min)(EaseOutCubic(rampUpTimer_ / rampUpTime_), 1.f);
    currentSpeed_ = baseSpeed_ * std::lerp(1.f, playerStatus->GetRailSpeedRate(), t);

    // 現在地の更新
    // 位置を座標で持たず「レール始点からの累積距離」で管理することで、
    // カーブでも速度が一定に保たれる(制御点の間隔に依存しない)。
    // 終端を超えるとセグメント検索が範囲外を指すため、全長でクランプする
    traveledDistance_ += currentSpeed_ * _deltaTime;
    traveledDistance_                               = (std::min)(railTotalLength_, traveledDistance_);
    std::pair<uint32_t, uint32_t> movedSegmentPoint = CalcDistanceSegmentIndex(railPoints->GetPoints(), traveledDistance_);
    currentRailPointIndex_                          = movedSegmentPoint.first;
    nextRailPointIndex_                             = movedSegmentPoint.second;

    // transform の更新
    transform->translate = CalcPointOnSplineByDistance(railPoints->GetPoints(), traveledDistance_) * railTransform->worldMat;
    transform->rotate    = Quaternion::LookAt(front, axisY);
    // 傾きの補正
    // 横入力に応じて機体をロールさせる演出。正式な軸回転の合成ではなく、
    // クォータニオンのZ成分を直接書き換える簡易的な方法を取っている。
    // 成分を直接いじると単位クォータニオンでなくなるため、直後に必ずnormalize()すること
    auto* effectControlParam = scene_->GetComponent<PlayerEffectControlParam>(playerEntityHandle_);
    if (effectControlParam) {
        float maxTiltAngle   = effectControlParam->GetMaxTiltOnRailRun();
        float tiltSpeed      = effectControlParam->GetTiltSpeedOnRailRun();
        t              = (playerInput->GetInputDirection()[X] + 1.0f) * 0.5f; // -1~1 を 0～1で表現
        transform->rotate[Z] = LerpByDeltaTime(transform->rotate[Z], std::lerp(-maxTiltAngle, maxTiltAngle, t), _deltaTime, tiltSpeed);
    }
    transform->rotate = transform->rotate.normalize();

    // プレイヤーの高さ補正
    // レール中心から機体の高さ分だけ持ち上げる。オフセットをそのまま足すのではなく
    // 機体の回転を掛けてから足すことで、傾いている時も機体の「真上」方向へずれる。
    // (ワールドY方向に固定すると、傾いた時にレールへめり込んで見える)
    Vec3f offset = transform->rotate.RotateVector(kPlayerOffsetOnRail);
    transform->translate += offset;

    rigidbody->SetVelocity(front * currentSpeed_);
}

void PlayerOnRailState::Finalize() {
    auto* playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);
    auto* playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* transform    = scene_->GetComponent<Transform>(playerEntityHandle_);
    auto* rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    playerStatus->SetupRailInterval();

    rigidbody->SetIsActive(true);
    rigidbody->SetMaxFallSpeed(defaultMaxFallDownSpeed_);

    //! TODO : RailJump State を作成する
    float inputX            = playerInput->GetInputDirection()[X];
    const Vec3f& jumpOffset = playerStatus->GetRailJumpOffset();
    Vec3f jumpVelo          = jumpOffset * currentSpeed_;

    // -1 ~ 1 を3等分 し、
    // -1$ ～ -0.333... の範囲なら 補正を左に
    // -0.333... ～ 0.333...の範囲なら 補正を正面に
    // 0.333... ～ 1 の範囲なら 補正を右に(通常状態)
    // 入力を3分割して「左・正面・右」の離脱方向に量子化しているのは、
    // スティックの傾き量をそのまま反映すると微妙な入力で飛び先がぶれ、
    // 隣のレールへ乗り移る操作が安定しないため
    if (inputX >= -1.f && inputX <= -kInputDirThird) {
        // 左入力: 既定(右向き)のX成分を反転して左へ飛ばす
        jumpVelo[X] *= -1.f;
    } else if (inputX > -kInputDirThird && inputX <= kInputDirThird) {
        // 正面入力: 左右へ飛ばさない。ただしX成分を単純に捨てると真上に跳ぶだけで
        // 飛距離が落ちるため、横方向の勢いの半分を上と前へ振り替えてから0にする
        jumpVelo += Vec3f(0.f, 0.5f, 0.5f) * jumpVelo[X];
        jumpVelo[X] = 0.f;
    }

    jumpVelo = transform->rotate.RotateVector(jumpVelo);
    rigidbody->SetVelocity(jumpVelo);
}

PlayerMoveState PlayerOnRailState::TransitionState() const {
    auto* playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);

    if (playerInput->IsRailJumpInput()) {
        return PlayerMoveState::FALL_DOWN;
    }

    if (traveledDistance_ >= railTotalLength_ - kEpsilon) {
        return PlayerMoveState::FALL_DOWN;
    }

    return this->GetState();
}
