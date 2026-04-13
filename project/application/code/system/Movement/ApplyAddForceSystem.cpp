#include "ApplyAddForceSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"

using namespace OriGine;

ApplyAddForceSystem::ApplyAddForceSystem() : ISystem(SystemCategory::Movement) {}
ApplyAddForceSystem::~ApplyAddForceSystem() {}

void ApplyAddForceSystem::Initialize() {
    // VelocityOverrideEvent を購読
    auto shared                                 = shared_from_this();
    std::weak_ptr<ApplyAddForceSystem> weakSelf = shared;
    addForceEventId_ =
        MessageBus::GetInstance()->Subscribe<AddForceEvent>(
            [weakSelf](const AddForceEvent& _event) {
                if (auto self = weakSelf.lock()) {
                    self->addForceEvents_.push_back(_event);
                }
            });
}

void ApplyAddForceSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<AddForceEvent>(addForceEventId_);
}

void ApplyAddForceSystem::Update() {
    for (const auto& event : addForceEvents_) {
        // Rigidbody コンポーネントを取得
        auto rigidbodyComp = GetComponent<Rigidbody>(event.rigidbodyHandle);
        if (!rigidbodyComp) {
            continue;
        }
        // 力を加算
        rigidbodyComp->SetVelocity(rigidbodyComp->GetVelocity() + event.force);
    }
    // 保留中のイベントをクリア
    addForceEvents_.clear();
}
