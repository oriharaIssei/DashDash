#include "SceneTransitionRequestSenderSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// event
#include "event/SceneChangeRequest.h"

/// ECS
// component
#include "component/scene/SceneChanger.h"

using namespace OriGine;

SceneTransitionRequestSenderSystem::SceneTransitionRequestSenderSystem()
    : ISystem(SystemCategory::Movement) {}
SceneTransitionRequestSenderSystem::~SceneTransitionRequestSenderSystem() {}

void SceneTransitionRequestSenderSystem::Initialize() {}
void SceneTransitionRequestSenderSystem::Finalize() {}

void SceneTransitionRequestSenderSystem::UpdateEntity(EntityHandle _handle) {
    auto& sceneChangerComps = GetComponents<SceneChanger>(_handle);

    if (sceneChangerComps.empty()) {
        return;
    }

    for (auto& changer : sceneChangerComps) {
        // シーン変更リクエストイベントを飛ばす
        if (changer.isChanged()) {
            MessageBus::GetInstance()->Emit<SceneChangeRequest>(SceneChangeRequest(changer.GetNextSceneName()));
        }
    }
}
