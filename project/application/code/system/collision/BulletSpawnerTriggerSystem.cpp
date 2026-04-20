#include "BulletSpawnerTriggerSystem.h"

#include "component/gimmick/BulletSpawner.h"
#include "component/gimmick/BulletSpawnerTrigger.h"

using namespace OriGine;

ICollisionTriggerComponent* BulletSpawnerTriggerSystem::GetTrigger(EntityHandle _handle) {
    return GetComponent<BulletSpawnerTrigger>(_handle);
}

void BulletSpawnerTriggerSystem::ApplyActivate(EntityHandle _targetHandle) {
    for (auto& spawner : GetComponents<BulletSpawner>(_targetHandle)) {
        spawner.PlayStart();
    }
}

void BulletSpawnerTriggerSystem::ApplyDeactivate(EntityHandle _targetHandle) {
    for (auto& spawner : GetComponents<BulletSpawner>(_targetHandle)) {
        spawner.PlayStop();
    }
}
