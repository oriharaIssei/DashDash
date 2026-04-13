#include "ObstacleSpawnEventTriggerSystem.h"

/// engine
#include "scene/Scene.h"

/// ECS
// component
#include "component/animation/DissolveAnimation.h"
#include "component/collision/collider/AABBCollider.h"
#include "component/gimmick/ObstacleSpawnGroupComponent.h"

using namespace OriGine;

ObstacleSpawnEventTriggerSystem::ObstacleSpawnEventTriggerSystem() : ISystem(SystemCategory::Collision) {}
ObstacleSpawnEventTriggerSystem::~ObstacleSpawnEventTriggerSystem() {}

void ObstacleSpawnEventTriggerSystem::Initialize() {}
void ObstacleSpawnEventTriggerSystem::Finalize() {}

void ObstacleSpawnEventTriggerSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto* obstacleGroup = GetComponent<ObstacleSpawnGroupComponent>(_handle);
    auto* collider      = GetComponent<AABBCollider>(_handle);

    if (!collider) {
        return;
    }

    bool isTriggered = false;
    for (const auto& [handle, state] : collider->GetCollisionStateMap()) {
        if (state == CollisionState::Enter) {
            // 衝突してたら発火
            for (EntityHandle obstacleHandle : obstacleGroup->GetObstacleHandles()) {
                auto* obstacleCollider = GetComponent<AABBCollider>(obstacleHandle);
                obstacleCollider->SetActive(true);

                auto* dissAnimation = GetComponent<DissolveAnimation>(obstacleHandle);
                if (!dissAnimation->IsPlaying()) {
                    dissAnimation->PlayStart();
                }
            }
            isTriggered = true;
            break;
        }
    }

    // 発火したら自分は削除予約
    if (isTriggered) {
        GetScene()->AddDeleteEntity(_handle);
    }
}
