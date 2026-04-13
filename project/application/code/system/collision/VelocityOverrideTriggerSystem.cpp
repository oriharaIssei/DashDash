#include "VelocityOverrideTriggerSystem.h"

/// stl
#include <unordered_set>

/// engine
#include "messageBus/MessageBus.h"

/// ECS
// component
#include "component/VelocityOverrideComponent.h"
#include "component/collision/collider/AABBCollider.h"
#include "component/collision/collider/ObbCollider.h"
#include "component/collision/collider/SphereCollider.h"
#include "component/physics/Rigidbody.h"

/// event
#include "event/VelocityOverrideEvent.h"

using namespace OriGine;

VelocityOverrideTriggerSystem::VelocityOverrideTriggerSystem() : ISystem(SystemCategory::Collision) {}
VelocityOverrideTriggerSystem::~VelocityOverrideTriggerSystem() {}

void VelocityOverrideTriggerSystem::Initialize() {}
void VelocityOverrideTriggerSystem::Finalize() {}

void VelocityOverrideTriggerSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto velocityOverrideComp = GetComponent<VelocityOverrideComponent>(_handle);
    if (!velocityOverrideComp) {
        return;
    }

    // ジャンプパッドと衝突したエンティティを取得
    auto& sphereColliders = GetComponents<SphereCollider>(_handle);
    auto& aabbColliders   = GetComponents<AABBCollider>(_handle);
    auto& obbColliders    = GetComponents<OBBCollider>(_handle);

    std::unordered_set<EntityHandle> collidedEntities;
    for (auto& collider : sphereColliders) {
        for (const auto& [otherHandle, state] : collider.GetCollisionStateMap()) {
            if (state == CollisionState::Enter) {
                collidedEntities.insert(otherHandle);
            }
        }
    }
    for (auto& collider : aabbColliders) {
        for (const auto& [otherHandle, state] : collider.GetCollisionStateMap()) {
            if (state == CollisionState::Enter) {
                collidedEntities.insert(otherHandle);
            }
        }
    }
    for (auto& collider : obbColliders) {
        for (const auto& [otherHandle, state] : collider.GetCollisionStateMap()) {
            if (state == CollisionState::Enter) {
                collidedEntities.insert(otherHandle);
            }
        }
    }

    // 衝突したエンティティにジャンプ力を加える
    for (const auto& otherHandle : collidedEntities) {
        auto rigidbodyComp = GetComponent<Rigidbody>(otherHandle);
        if (rigidbodyComp) {
            // 速度を強制的に上書き
            VelocityOverrideEvent event(rigidbodyComp->GetHandle(), velocityOverrideComp->forcedMovementVector_, velocityOverrideComp->isOverrideAxis_);
            MessageBus::GetInstance()->Emit<VelocityOverrideEvent>(event);
        }
    }
}
