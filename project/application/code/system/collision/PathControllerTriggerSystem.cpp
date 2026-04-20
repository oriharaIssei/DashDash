#include "PathControllerTriggerSystem.h"

#include "component/gimmick/PathController.h"
#include "component/gimmick/PathControllerTrigger.h"

using namespace OriGine;

ICollisionTriggerComponent* PathControllerTriggerSystem::GetTrigger(EntityHandle _handle) {
    return GetComponent<PathControllerTrigger>(_handle);
}

void PathControllerTriggerSystem::ApplyActivate(EntityHandle _targetHandle) {
    for (auto& pc : GetComponents<PathController>(_targetHandle)) {
        pc.isPlaying = true;
    }
}

void PathControllerTriggerSystem::ApplyDeactivate(EntityHandle _targetHandle) {
    for (auto& pc : GetComponents<PathController>(_targetHandle)) {
        pc.isPlaying = false;
    }
}
