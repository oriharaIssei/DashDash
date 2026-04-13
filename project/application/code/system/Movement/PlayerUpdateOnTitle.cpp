#include "PlayerUpdateOnTitle.h"

/// engine
#include "Engine.h"

// component
#include "component/physics/Rigidbody.h"
#include "component/player/PlayerInput.h"
#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerDashState.h"
#include "component/player/state/PlayerState.h"
#include "component/transform/Transform.h"

/// math
#include "math/Interpolation.h"
#include "math/MathEnv.h"

using namespace OriGine;

PlayerUpdateOnTitle::PlayerUpdateOnTitle() : ISystem(OriGine::SystemCategory::Movement) {}
PlayerUpdateOnTitle::~PlayerUpdateOnTitle() {}

void PlayerUpdateOnTitle::Initialize() {
    // 特に初期化処理は不要
}
void PlayerUpdateOnTitle::Finalize() {
    // 特に終了処理は不要
}

void PlayerUpdateOnTitle::UpdateEntity(OriGine::EntityHandle _handle) {
    auto* playerInput = GetComponent<PlayerInput>(_handle);
    if (playerInput == nullptr) {
        LOG_ERROR("PlayerInput component is missing.");
        return;
    }
    auto* playerState = GetComponent<PlayerState>(_handle);
    if (playerState == nullptr) {
        LOG_ERROR("PlayerState component is missing.");
        return;
    }
    auto* playerStatus = GetComponent<PlayerStatus>(_handle);
    if (playerStatus == nullptr) {
        LOG_ERROR("PlayerStatus component is missing.");
        return;
    }

    auto& stateFlag = playerState->GetStateFlagRef();
    stateFlag.Sync();

    // ジャンプ入力を無効化
    playerInput->SetJumpInput(false);

    // タイトル画面では常に地面に接地している状態にする
    playerState->OnCollisionGround();

    // 更新処理 ほぼ PlayerDashState と同じ
    float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");
    auto* transform = GetComponent<OriGine::Transform>(_handle);
    auto* rigidbody = GetComponent<Rigidbody>(_handle);

    playerStatus->minusGearUpCoolTime(deltaTime);

    int32_t gearLevel = playerState->GetGearLevel();
    // ギアレベルが最大に達していない場合、
    if (gearLevel < kMaxPlayerGearLevel - 1) {
        // クールタイムが0以下になったらギアレベルを上げる
        if (playerStatus->GetGearUpCoolTime() <= 0.f) {
            stateFlag.SetCurrent(stateFlag.Current() | PlayerStateFlag::GEAR_UP);

            ++gearLevel;
            playerState->SetGearLevel(gearLevel);

            playerStatus->SetGearUpCoolTime(playerStatus->CalculateCoolTimeByGearLevel(gearLevel));

            playerStatus->SetCurrentMaxSpeed(playerStatus->CalculateSpeedByGearLevel(gearLevel));
        }
    }

    // 速度更新
    Vec3f worldInputDir    = playerInput->CalculateWorldInputDirection(Quaternion::Identity());
    Vec3f forwardDirection = playerStatus->ComputeSmoothedDirection(worldInputDir, rigidbody, transform, deltaTime);
    transform->rotate      = Quaternion::LookAt(forwardDirection, axisY);
    rigidbody->SetVelocity(playerStatus->GetCurrentMaxSpeed() * forwardDirection);

    if (playerInput->GetInputDirection().length() >= kEpsilon) {
        OriGine::Vec3f newVelo = rigidbody->GetVelocity() + rigidbody->GetAcceleration() * deltaTime;
        if (newVelo.lengthSq() >= rigidbody->GetMaxXZSpeed()) {
            newVelo = newVelo.normalize() * rigidbody->GetMaxXZSpeed();
        }
        rigidbody->SetVelocity(newVelo);
    } else {
        constexpr float kDecelerationRate = 1.6f;
        /// 初期化処理
        // 入力方向をリセット
        playerInput->SetWorldInputDirection({0.f, 0.f, 0.f});
        // 速度をゼロにする
        rigidbody->SetAcceleration({0.f, 0.0f, 0.0f});
        playerStatus->SetCurrentMaxSpeed(0.0f);
        // ギアアップのクールタイム&ギアレベルをリセット
        playerStatus->SetGearUpCoolTime(playerStatus->GetBaseGearupCoolTime());
        playerState->SetGearLevel(kDefaultPlayerGearLevel);

        // 速度を減衰させる
        rigidbody->SetVelocity(LerpByDeltaTime(rigidbody->GetVelocity(), OriGine::Vec3f(), deltaTime, kDecelerationRate));
    }
    transform->UpdateMatrix();
}
