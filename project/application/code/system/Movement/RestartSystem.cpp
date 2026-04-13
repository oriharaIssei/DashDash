#include "RestartSystem.h"

/// engine
#include "messageBus/MessageBus.h"

#include "scene/Scene.h"
#include "scene/SceneManager.h"

/// event
#include "event/RestartEvent.h"
#include "event/SceneChangeRequest.h"

// system
#include "system/SystemRunner.h"

#include "system/transition/CameraMotionBobSystem.h"
#include "system/transition/TransitionPlayerState.h"

using namespace OriGine;

RestartSystem::RestartSystem() : ISystem(SystemCategory::Movement) {}
RestartSystem::~RestartSystem() {}

void RestartSystem::Initialize() {
    // リスタートイベントを購読
    auto shared = shared_from_this();
    std::weak_ptr<RestartSystem> weakSelf = shared;

    restartEventId_ =
        MessageBus::GetInstance()->Subscribe<RestartEvent>(
            [weakSelf](const RestartEvent&) {
                if (auto self = weakSelf.lock()) {
                    self->isRestarting_ = true;
                }
            });
}

void RestartSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<RestartEvent>(restartEventId_);
}

void RestartSystem::Update() {
    if (!isRestarting_) {
        return;
    }
    // リスタート処理
    auto* currentScene = GetScene();
    // メッセージを飛ばす
    MessageBus::GetInstance()->Emit<SceneChangeRequest>(SceneChangeRequest{currentScene->GetName()});

    // effectを流す
    auto* systemRunner = currentScene->GetSystemRunnerRef();
    systemRunner->SetCategoryActivity(SystemCategory::StateTransition, true);
    systemRunner->SetCategoryActivity(SystemCategory::Effect, true);

    systemRunner->DeactivateSystem<TransitionPlayerState>();
    systemRunner->DeactivateSystem<CameraMotionBobSystem>();
}
