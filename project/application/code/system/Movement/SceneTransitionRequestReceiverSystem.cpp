#include "SceneTransitionRequestReceiverSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// event
#include "event/SceneChangeRequest.h"

/// ECS
// component
#include "component/scene/SceneChanger.h"

using namespace OriGine;

SceneTransitionRequestReceiverSystem::SceneTransitionRequestReceiverSystem()
    : ISystem(SystemCategory::Movement) {}

SceneTransitionRequestReceiverSystem::~SceneTransitionRequestReceiverSystem() = default;

void SceneTransitionRequestReceiverSystem::Initialize() {
    // リスタートイベントを購読
    auto shared                                                  = shared_from_this();
    std::weak_ptr<SceneTransitionRequestReceiverSystem> weakSelf = shared;

    sceneChangeRequestEventId_ =
        MessageBus::GetInstance()->Subscribe<SceneChangeRequest>(
            [weakSelf](const SceneChangeRequest& _req) {
                if (auto self = weakSelf.lock()) {
                    self->isSceneChangeRequested_ = true;
                    self->targetSceneName_        = _req.targetSceneName;
                }
            });
}
void SceneTransitionRequestReceiverSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<SceneChangeRequest>(sceneChangeRequestEventId_);
}

void SceneTransitionRequestReceiverSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    if (!isSceneChangeRequested_) {
        return;
    }
    // フラグをリセット
    isSceneChangeRequested_ = false;

    SceneChanger* sceneChangerComp = GetComponent<SceneChanger>(_handle);

    if (!sceneChangerComp) {
        return;
    }

    // シーン変更リクエストイベントを飛ばす
    sceneChangerComp->SetNextSceneName(targetSceneName_);
    sceneChangerComp->ChangeScene();
}
