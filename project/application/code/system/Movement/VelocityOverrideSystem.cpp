#include "VelocityOverrideSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"

using namespace OriGine;

VelocityOverrideSystem::VelocityOverrideSystem() : ISystem(SystemCategory::Movement) {}
VelocityOverrideSystem::~VelocityOverrideSystem() {}

void VelocityOverrideSystem::Initialize() {
    // VelocityOverrideEvent を購読
    auto shared                                    = shared_from_this();
    std::weak_ptr<VelocityOverrideSystem> weakSelf = shared;
    velocityOverrideEventId_ =
        MessageBus::GetInstance()->Subscribe<VelocityOverrideEvent>(
            [weakSelf](const VelocityOverrideEvent& _event) {
                if (auto self = weakSelf.lock()) {
                    self->velocityOverrides_.push_back(_event);
                }
            });
}

void VelocityOverrideSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<VelocityOverrideEvent>(velocityOverrideEventId_);
}

void VelocityOverrideSystem::Update() {
    for (const auto& event : velocityOverrides_) {
        // Rigidbody コンポーネントを取得
        auto rigidbodyComp = GetComponent<Rigidbody>(event.rigidbodyHandle);
        if (!rigidbodyComp) {
            continue;
        }
        // 速度を上書き
        for (int i = 0; i < Vec3f::dim; ++i) {
            if (event.isOverrideAxis[i]) {
                rigidbodyComp->SetVelocity(i, event.newVelocity[i]);
            }
        }
    }
    // 保留中のイベントをクリア
    velocityOverrides_.clear();
}
