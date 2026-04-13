#include "AddForceTriggerSystem.h"

/// stl
#include <unordered_set>

/// engine
#include "Engine.h"
#include "messageBus/MessageBus.h"

/// ECS
// component
#include "component/AddForceComponent.h"
#include "component/collision/collider/AABBCollider.h"
#include "component/collision/collider/ObbCollider.h"
#include "component/collision/collider/SphereCollider.h"
#include "component/physics/Rigidbody.h"

/// event
#include "event/AddForceEvent.h"

using namespace OriGine;

AddForceTriggerSystem::AddForceTriggerSystem() : ISystem(SystemCategory::Collision) {}
AddForceTriggerSystem::~AddForceTriggerSystem() {}

void AddForceTriggerSystem::Initialize() {}
void AddForceTriggerSystem::Finalize() {}

void AddForceTriggerSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto addForceComp = GetComponent<AddForceComponent>(_handle);
    if (!addForceComp) {
        return;
    }

    const bool isWhileColliding = (addForceComp->triggerMode_ == AddForceComponent::TriggerMode::WhileColliding);

    // コライダーから衝突エンティティを収集
    auto& sphereColliders = GetComponents<SphereCollider>(_handle);
    auto& aabbColliders   = GetComponents<AABBCollider>(_handle);
    auto& obbColliders    = GetComponents<OBBCollider>(_handle);

    std::unordered_set<EntityHandle> collidedEntities;
    auto collectCollisions = [&](auto& _colliders) {
        for (auto& collider : _colliders) {
            for (const auto& [otherHandle, state] : collider.GetCollisionStateMap()) {
                if (state == CollisionState::Enter) {
                    collidedEntities.insert(otherHandle);
                } else if (isWhileColliding && state == CollisionState::Stay) {
                    collidedEntities.insert(otherHandle);
                }
            }
        }
    };
    collectCollisions(sphereColliders);
    collectCollisions(aabbColliders);
    collectCollisions(obbColliders);

    // 衝突したエンティティに力を加える
    for (const auto& otherHandle : collidedEntities) {
        auto rigidbodyComp = GetComponent<Rigidbody>(otherHandle);
        if (rigidbodyComp) {
            Vec3f force = addForceComp->addForce_;

            // WhileCollidingモードでは継続力なので deltaTime を掛ける
            if (isWhileColliding) {
                float deltaTime;
                if (rigidbodyComp->IsUsingLocalDeltaTime()) {
                    deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime(rigidbodyComp->GetLocalDeltaTimeName());
                } else {
                    deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetDeltaTime();
                }
                force = force * deltaTime;
            }

            AddForceEvent event(rigidbodyComp->GetHandle(), force);
            MessageBus::GetInstance()->Emit<AddForceEvent>(event);
        }
    }
}
