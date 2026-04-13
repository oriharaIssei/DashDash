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

PlayerOnRailState::PlayerOnRailState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle)
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
    Vec3f playerPosOnRail                           = transform->translate * railTransform->worldMat.inverse();
    std::pair<uint32_t, uint32_t> firstSegmentPoint = CalcPointSegmentIndex(railPoints->points, playerPosOnRail);
    currentRailPointIndex_                          = firstSegmentPoint.first;
    nextRailPointIndex_                             = firstSegmentPoint.second;

    railTotalLength_ = CalcSplineLength(railPoints->points);

    // 正確な距離を計算：最も近いセグメントまでの累積距離 + セグメント内の射影距離
    float accumulatedLength = 0.0f;
    for (uint32_t i = 0; i < currentRailPointIndex_; ++i) {
        accumulatedLength += Vec3f(railPoints->points[i + 1] - railPoints->points[i]).length();
    }
    // 現在のセグメント内での射影位置を計算
    const Vec3f& p1    = railPoints->points[currentRailPointIndex_];
    const Vec3f& p2    = railPoints->points[nextRailPointIndex_];
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

    if (currentRailPointIndex_ > railPoints->points.size()) {
        isOutOfRail_ = false;
        return;
    }

    // 使用する一時変数
    const Vec3f& currentPoint = railPoints->points[currentRailPointIndex_];
    const Vec3f& nextPoint    = railPoints->points[nextRailPointIndex_];
    currentDirection_         = nextPoint - currentPoint;
    Vec3f front               = currentDirection_.normalize();

    // 速度
    rampUpTimer_ += _deltaTime;
    float t       = (std::min)(EaseOutCubic(rampUpTimer_ / rampUpTime_), 1.f);
    currentSpeed_ = baseSpeed_ * std::lerp(1.f, playerStatus->GetRailSpeedRate(), t);

    // 現在地の更新
    traveledDistance_ += currentSpeed_ * _deltaTime;
    traveledDistance_                               = (std::min)(railTotalLength_, traveledDistance_);
    std::pair<uint32_t, uint32_t> movedSegmentPoint = CalcDistanceSegmentIndex(railPoints->points, traveledDistance_);
    currentRailPointIndex_                          = movedSegmentPoint.first;
    nextRailPointIndex_                             = movedSegmentPoint.second;

    // transform の更新
    transform->translate = CalcPointOnSplineByDistance(railPoints->points, traveledDistance_) * railTransform->worldMat;
    transform->rotate    = Quaternion::LookAt(front, axisY);
    // 傾きの補正
    auto* effectControlParam = scene_->GetComponent<PlayerEffectControlParam>(playerEntityHandle_);
    if (effectControlParam) {
        float maxTiltAngle   = effectControlParam->GetMaxTiltOnRailRun();
        float tiltSpeed      = effectControlParam->GetTiltSpeedOnRailRun();
        t              = (playerInput->GetInputDirection()[X] + 1.0f) * 0.5f; // -1~1 を 0～1で表現
        transform->rotate[Z] = LerpByDeltaTime(transform->rotate[Z], std::lerp(-maxTiltAngle, maxTiltAngle, t), _deltaTime, tiltSpeed);
    }
    transform->rotate = transform->rotate.normalize();

    // プレイヤーの高さ補正
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
    if (inputX >= -1.f && inputX <= -0.3333f) {
        jumpVelo[X] *= -1.f;
    } else if (inputX > -0.333f && inputX <= 0.3333f) {
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
