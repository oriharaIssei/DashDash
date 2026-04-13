#include "PlayerStateOverrideSystem.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"
// input
#include "input/GamepadInput.h"

/// ECS
// component
#include "component/collision/CollisionPushBackInfo.h"
#include "component/PlayerStateOverrideCondition.h"
#include "component/renderer/Sprite.h"

using namespace OriGine;

PlayerStateOverrideSystem::PlayerStateOverrideSystem() : ISystem(SystemCategory::StateTransition) {}
PlayerStateOverrideSystem::~PlayerStateOverrideSystem() {}

void PlayerStateOverrideSystem::Initialize() {
    Engine* engine = Engine::GetInstance();
    engine->GetDeltaTimer()->SetTimeScale("Player", 1.f);
    engine->GetDeltaTimer()->SetTimeScale("Effect", 1.f);
    engine->GetDeltaTimer()->SetTimeScale("Camera", 1.f);
}
void PlayerStateOverrideSystem::Finalize() {
    Engine* engine = Engine::GetInstance();
    engine->GetDeltaTimer()->SetTimeScale("Player", 1.f);
    engine->GetDeltaTimer()->SetTimeScale("Effect", 1.f);
    engine->GetDeltaTimer()->SetTimeScale("Camera", 1.f);
}

void PlayerStateOverrideSystem::Update() {
    isTimeScaled_ = false;

    ISystem::Update();

    if (isTimeScaled_) {
        constexpr float kTimeRate = 0.1f;
        Engine* engine            = Engine::GetInstance();
        engine->GetDeltaTimer()->SetTimeScale("Player", kTimeRate);
        engine->GetDeltaTimer()->SetTimeScale("Effect", kTimeRate);
        engine->GetDeltaTimer()->SetTimeScale("Camera", kTimeRate);
    } else {
        Engine* engine = Engine::GetInstance();
        engine->GetDeltaTimer()->SetTimeScale("Player", 1.f);
        engine->GetDeltaTimer()->SetTimeScale("Effect", 1.f);
        engine->GetDeltaTimer()->SetTimeScale("Camera", 1.f);
    }
}

void PlayerStateOverrideSystem::UpdateEntity(EntityHandle _handle) {
    auto pushBackInfo      = GetComponent<CollisionPushBackInfo>(_handle);
    auto overrideCondition = GetComponent<PlayerStateOverrideCondition>(_handle);

    auto keyUi     = GetComponent<SpriteRenderer>(_handle, 0);
    auto gamepadUi = GetComponent<SpriteRenderer>(_handle, 1);
    if (!keyUi || !gamepadUi) {
        return;
    }
    // UIを非表示にする
    keyUi->SetIsRender(false);
    gamepadUi->SetIsRender(false);

    if (!pushBackInfo || !overrideCondition) {
        return;
    }

    for (auto& [id, info] : pushBackInfo->GetCollisionInfoMap()) {
        auto playerState = GetComponent<PlayerState>(id);
        if (!playerState) {
            continue;
        }

        // 状態が一致しなければ、時間を止める
        if (!overrideCondition->overrideConditions.HasFlag(playerState->GetStateEnum())) {
            isTimeScaled_ = true;

            // 対応した spriteを表示
            if (GetScene()->GetGamepadInput()->IsActive()) {
                keyUi->SetIsRender(false);
                gamepadUi->SetIsRender(true);
            } else {
                keyUi->SetIsRender(true);
                gamepadUi->SetIsRender(false);
            }

            return;
        }
    }
}
