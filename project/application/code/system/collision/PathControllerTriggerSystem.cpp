#include "PathControllerTriggerSystem.h"

#include "component/gimmick/PathController.h"
#include "component/gimmick/PathControllerTrigger.h"

using namespace OriGine;

ICollisionTriggerComponent* PathControllerTriggerSystem::GetTrigger(const EntityHandle& _handle) {
    return GetComponent<PathControllerTrigger>(_handle);
}

void PathControllerTriggerSystem::ApplyActivate(const EntityHandle& _targetHandle) {
    for (auto& pc : GetComponents<PathController>(_targetHandle)) {
        pc.SetPlaying(true);
    }
}

void PathControllerTriggerSystem::ApplyDeactivate(const EntityHandle& _targetHandle) {
    for (auto& pc : GetComponents<PathController>(_targetHandle)) {
        pc.SetPlaying(false);
    }
}
