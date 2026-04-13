#include "PlayerIdleState.h"

/// component
#include "component/animation/SkinningAnimationComponent.h"
#include "component/effect/particle/emitter/ParticleSystem.h"
#include "component/physics/Rigidbody.h"

#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

#include "component/player/PlayerConfig.h"

/// math
#include "MathEnv.h"

using namespace OriGine;

void PlayerIdleState::Initialize() {
    auto playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* state       = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto rigidbody    = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto playerInput  = scene_->GetComponent<PlayerInput>(playerEntityHandle_);

    /// 初期化処理
    // 入力方向をリセット
    playerInput->SetWorldInputDirection({0.f, 0.f, 0.f});
    // 速度をゼロにする
    rigidbody->SetAcceleration({0.f, 0.0f, 0.0f});
    playerStatus->SetCurrentMaxSpeed(0.0f);
    // ギアアップのクールタイム&ギアレベルをリセット
    playerStatus->SetGearUpCoolTime(playerStatus->GetBaseGearupCoolTime());
    state->SetGearLevel(kDefaultPlayerGearLevel);

}

void PlayerIdleState::Update(float _deltaTime) {
    auto* rigidbody = scene_->GetComponent<Rigidbody>(playerEntityHandle_);
    auto* status    = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);

    float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");

    // 減速
    Vec3f velo = rigidbody->GetVelocity();
    if (velo.lengthSq() > kEpsilon) {
        velo *= std::powf(status->GetDecelerationFactor(), deltaTime); // 減速係数を時間で累乗して適用(1秒単位で適応する)
    } else {
        velo = Vec3f(0.f, velo[Y], 0.f); // 完全に停止させる。ただしY軸の速度は落とさない
    }

    rigidbody->SetVelocity(velo);

    // 落下時間を更新
    auto* state = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    if (state->IsOnGround()) {
        fallDownTimer_ = kFallDownThresholdTime_;
    } else {
        fallDownTimer_ -= _deltaTime;
    }

}

void PlayerIdleState::Finalize() {
    auto playerStatus = scene_->GetComponent<PlayerStatus>(playerEntityHandle_);
    auto* rigidbody   = scene_->GetComponent<Rigidbody>(playerEntityHandle_);

    playerStatus->SetCurrentMaxSpeed(playerStatus->GetBaseSpeed());
    // Jump がおかしくなるため しっかりと ゼロ にする
    rigidbody->SetVelocity({0.0f, 0.0f, 0.0f});
}

PlayerMoveState PlayerIdleState::TransitionState() const {
    auto state       = scene_->GetComponent<PlayerState>(playerEntityHandle_);
    auto playerInput = scene_->GetComponent<PlayerInput>(playerEntityHandle_);

    if (state->IsOnGround()) {
        if (playerInput->IsJumpInput()) {
            return PlayerMoveState::JUMP;
        }
        if (playerInput->GetInputDirection().lengthSq() > 0.f) {
            return PlayerMoveState::DASH;
        }

        return PlayerMoveState::IDLE;
    } else {
        if (fallDownTimer_ <= 0.f) {
            return PlayerMoveState::FALL_DOWN;
        }
    }
    return PlayerMoveState::IDLE;
}
