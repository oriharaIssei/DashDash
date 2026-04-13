#include "PauseMainSceneSystem.h"

/// component
#include "component/scene/SubScene.h"
/// system
#include "system/SystemRunner.h"

#include "system/movement/FollowCameraUpdateSystem.h"
#include "system/movement/MoveSystemByRigidBody.h"
#include "system/movement/PathControllerSystem.h"
#include "system/movement/PlayerMoveSystem.h"
#include "system/render/SpriteRenderSystem.h"
#include "system/Transition/ApplyMouseConditionSystem.h"
#include "system/Transition/ButtonGroupSystem.h"
#include "system/Transition/TimerCountDown.h"
#include "system/Transition/TimerCountUp.h"
#include "system/Transition/TransitionPlayerState.h"

/// util
#include <nameof.h>

using namespace OriGine;

PauseMainSceneSystem::PauseMainSceneSystem() : ISystem(OriGine::SystemCategory::Movement) {}
PauseMainSceneSystem::~PauseMainSceneSystem() {}

void PauseMainSceneSystem::Initialize() {
    deactivateSystemNames_ = {
        nameof<PlayerMoveSystem>(),
        nameof<MoveSystemByRigidBody>(),
        nameof<FollowCameraUpdateSystem>(),
        nameof<ApplyMouseConditionSystem>(),
        nameof<ButtonGroupSystem>(),
        nameof<TimerCountDown>(),
        nameof<TimerCountUp>(),
        nameof<TransitionPlayerState>(),
        nameof<SpriteRenderSystem>(),
        nameof<PathControllerSystem>(),
    };
}
void PauseMainSceneSystem::Finalize() {}

void PauseMainSceneSystem::Update() {
    if (entities_.empty()) {
        return;
    }

    EraseDeadEntity();

    auto systemRunner = GetScene()->GetSystemRunnerRef();
    isPausing_        = false;

    // ゲームのUpdateを再開
    systemRunner->SetCategoryActivity(SystemCategory::Input, true);
    systemRunner->SetCategoryActivity(SystemCategory::StateTransition, true);
    systemRunner->SetCategoryActivity(SystemCategory::Collision, true);
    systemRunner->SetCategoryActivity(SystemCategory::Effect, true);

    for (const auto& systemName : deactivateSystemNames_) {
        systemRunner->ActivateSystem(systemName);
    }

    for (auto& entityHandle : entities_) {
        UpdateEntity(entityHandle);
    }

    if (isPausing_) {
        // ポーズ中はゲームのUpdateを停止
        systemRunner->SetCategoryActivity(SystemCategory::Input, false);
        systemRunner->SetCategoryActivity(SystemCategory::Collision, false);
        systemRunner->SetCategoryActivity(SystemCategory::Effect, false);

        for (const auto& systemName : deactivateSystemNames_) {
            systemRunner->DeactivateSystem(systemName);
        }
    }
}

void PauseMainSceneSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto subScene = GetComponent<SubScene>(_handle);
    if (!subScene) {
        return;
    }

    // Pauseしたいシーンがアクティブならポーズ状態にする
    isPausing_ |= subScene->IsActive();
}
