#include "AttractModeSystem.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"
// input
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"
#include "input/MouseInput.h"

/// ECS
// component
#include "component/animation/SpriteAnimation.h"
#include "component/renderer/Sprite.h"

/// math
#include "MathEnv.h"

using namespace OriGine;

AttractModeSystem::AttractModeSystem()
    : ISystem(SystemCategory::StateTransition) {}

void AttractModeSystem::Initialize() {
    isSceneInitializing_ = true;
}
void AttractModeSystem::Finalize() {}

void AttractModeSystem::Update() {
    GamepadInput* gamepadInput   = GetScene()->GetGamepadInput();
    KeyboardInput* keyboardInput = GetScene()->GetKeyboardInput();
    MouseInput* mouseInput       = GetScene()->GetMouseInput();

    if (isSceneInitializing_) {
        sceneInitializationTimer_ += Engine::GetInstance()->GetDeltaTime();
        if (sceneInitializationTimer_ >= kSceneInitializationDelay_) {
            isSceneInitializing_ = false;
            idleTimer_           = kAttractModeThreshold_;
            isAttractModeActive_ = true;
        }
    } else {
        // いずれかの入力があるか
        bool isAnyInput = (gamepadInput && gamepadInput->IsActive() && gamepadInput->IsPressAny())
                          || (keyboardInput && keyboardInput->IsPressAnyKey())
                          || (mouseInput && mouseInput->IsPressAnyButton() && (mouseInput->GetVelocity().lengthSq() > kEpsilon));

        // 前回の状態を保存
        previouslyActive_ = isAttractModeActive_;

        // 状態・タイマーの更新
        if (isAnyInput) {
            idleTimer_           = 0.f;
            isAttractModeActive_ = false;
        } else {
            if (!isAttractModeActive_) {
                idleTimer_ += Engine::GetInstance()->GetDeltaTime();
                if (idleTimer_ >= kAttractModeThreshold_) {
                    isAttractModeActive_ = true;
                }
            }
        }
    }

    // 状態変化の検出
    ISystem::Update();
}

void AttractModeSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    constexpr int32_t kEnterSpriteAnimationIndex = 0;
    constexpr int32_t kExitSpriteAnimationIndex  = 1;
    auto* enterAnim                              = GetComponent<SpriteAnimation>(_handle, kEnterSpriteAnimationIndex);
    auto* exitAnim                               = GetComponent<SpriteAnimation>(_handle, kExitSpriteAnimationIndex);

    if (!enterAnim || !exitAnim) {
        return;
    }

    auto& sprites = GetComponents<SpriteRenderer>(_handle);

    // 0 を Enter / 1 を Exit として、状態変化のトリガーを検出
    // アトラクションモードのトリガーでアニメーションを再生 & スプライトを表示
    if (IsTriggered() || isSceneInitializing_) {
        if (enterAnim) {
            enterAnim->PlayStart();
        }
    } else if (IsReleased()) {
        if (exitAnim) {
            exitAnim->PlayStart();
        }
    }

    for (auto& sprite : sprites) {
        sprite.SetIsRender(isSceneInitializing_ || isAttractModeActive_ || exitAnim->IsPlaying());
    }
}

bool AttractModeSystem::IsTriggered() const {
    return isAttractModeActive_ && !previouslyActive_;
}

bool AttractModeSystem::IsReleased() const {
    return !isAttractModeActive_ && previouslyActive_;
}
