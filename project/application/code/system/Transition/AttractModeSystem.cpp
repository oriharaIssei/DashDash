#include "AttractModeSystem.h"

/// engine
#include "Engine.h"

/// ECS
// component
#include "component/animation/SpriteAnimation.h"
#include "component/renderer/Sprite.h"

using namespace OriGine;

AttractModeSystem::AttractModeSystem()
    : ISystem(SystemCategory::StateTransition)
    , activeState_(nullptr, &idleState_)
    , idleState_(nullptr, kAttractModeThreshold_, &activeState_)
    , initializingState_(kSceneInitializationDelay_, &activeState_) {}

void AttractModeSystem::Initialize() {
    activeState_ = AttractActiveState(GetScene(), &idleState_);
    idleState_   = AttractIdleState(GetScene(), kAttractModeThreshold_, &activeState_);

    currentState_  = &initializingState_;
    previousState_ = currentState_;
    currentState_->Enter();
}

void AttractModeSystem::Finalize() {}

void AttractModeSystem::Update() {
    const float deltaTime = Engine::GetInstance()->GetDeltaTime();
    // 現在の状態を更新
    currentState_->Update(deltaTime);

    previousState_ = currentState_;
    // 遷移条件を満たしていれば次の状態へ切り替える
    IAttractModeState* next = currentState_->Transition();
    if (next) {
        currentState_ = next;
        currentState_->Enter();
    }

    ISystem::Update();
}

void AttractModeSystem::UpdateEntity(const EntityHandle& _handle) {
    constexpr int32_t kEnterSpriteAnimationIndex = 0;
    constexpr int32_t kExitSpriteAnimationIndex  = 1;
    auto* enterAnim                              = GetComponent<SpriteAnimation>(_handle, kEnterSpriteAnimationIndex);
    auto* exitAnim                               = GetComponent<SpriteAnimation>(_handle, kExitSpriteAnimationIndex);

    if (!enterAnim || !exitAnim) {
        return;
    }

    auto& sprites = GetComponents<SpriteRenderer>(_handle);

    bool wasActive = previousState_ && previousState_->IsActive();
    bool isActive  = currentState_->IsActive();

    // Active に遷移した瞬間 → Enter アニメーション
    if (isActive && !wasActive) {
        enterAnim->PlayStart();
    }
    // Active から離れた瞬間 → Exit アニメーション
    else if (!isActive && wasActive) {
        exitAnim->PlayStart();
    }

    for (auto& sprite : sprites) {
        sprite.SetIsRender(currentState_->ShouldRender() || exitAnim->IsPlaying());
    }
}
