#include "BulletSpawnerTriggerSystem.h"

/// ECS
// component
#include "component/collision/collider/AABBCollider.h"
#include "component/gimmick/BulletSpawner.h"
#include "component/gimmick/BulletSpawnerTrigger.h"

using namespace OriGine;

void BulletSpawnerTriggerSystem::UpdateEntity(EntityHandle _handle) {
    auto* trigger  = GetComponent<BulletSpawnerTrigger>(_handle);
    auto* collider = GetComponent<AABBCollider>(_handle);

    if (!trigger || !collider) {
        return;
    }

    // 衝突 Enter を検知
    bool isTriggered = false;
    for (const auto& [handle, state] : collider->GetCollisionStateMap()) {
        if (state == CollisionState::Enter) {
            isTriggered = true;
            break;
        }
    }

    if (!isTriggered) {
        return;
    }

    // ターゲットの BulletSpawner を制御
    for (EntityHandle targetHandle : trigger->GetTargetHandles()) {
        auto& spawners = GetComponents<BulletSpawner>(targetHandle);
        for (auto& spawner : spawners) {
            switch (trigger->GetMode()) {
            case BulletSpawnerTrigger::Mode::Activate:
                spawner.PlayStart();
                break;
            case BulletSpawnerTrigger::Mode::Deactivate:
                spawner.PlayStop();
                break;
            }
        }
    }
}
