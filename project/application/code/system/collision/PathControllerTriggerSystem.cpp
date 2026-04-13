#include "PathControllerTriggerSystem.h"

/// ECS
// component
#include "component/collision/collider/AABBCollider.h"
#include "component/gimmick/PathController.h"
#include "component/gimmick/PathControllerTrigger.h"

using namespace OriGine;

void PathControllerTriggerSystem::UpdateEntity(EntityHandle _handle) {
    auto* trigger  = GetComponent<PathControllerTrigger>(_handle);
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

    // ターゲットの PathController を制御
    for (EntityHandle targetHandle : trigger->GetTargetHandles()) {
        auto& pathControllers = GetComponents<PathController>(targetHandle);
        for (auto& pc : pathControllers) {
            switch (trigger->GetMode()) {
            case PathControllerTrigger::Mode::Activate:
                pc.isPlaying = true;
                break;
            case PathControllerTrigger::Mode::Deactivate:
                pc.isPlaying = false;
                break;
            }
        }
    }
}
