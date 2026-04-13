#include "GameFailedSceneLaunchSystem.h"

/// engine
#include "messageBus/MessageBus.h"
#include "scene/Scene.h"

/// event
#include "event/GamefailedEvent.h"

/// ECS
// component
#include "component/scene/SubScene.h"

using namespace OriGine;

GameFailedSceneLaunchSystem::GameFailedSceneLaunchSystem() : ISystem(SystemCategory::StateTransition) {}
GameFailedSceneLaunchSystem::~GameFailedSceneLaunchSystem() {}

void GameFailedSceneLaunchSystem::Initialize() {
    gameFailedEventId_ = MessageBus::GetInstance()->Subscribe<GamefailedEvent>([this](const GamefailedEvent& /*_event*/) {
        isLaunched_ = true;
    });
}
void GameFailedSceneLaunchSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<GamefailedEvent>(gameFailedEventId_);
}

void GameFailedSceneLaunchSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    if (!isLaunched_) {
        return;
    }
    isLaunched_ = false; // 一度だけ処理するためにフラグをリセット

    // ゲーム失敗メッセージを受け取ったら ゲーム失敗シーンを起動する
    auto& subSceneComps = GetComponents<SubScene>(_handle);
    if (subSceneComps.empty()) {
        return;
    }

    for (auto& subScene : subSceneComps) {
        if (!subScene.IsActive()) {
            subScene.Activate();
        }
    }
}
