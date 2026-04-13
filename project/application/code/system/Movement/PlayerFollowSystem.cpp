#include "PlayerFollowSystem.h"

/// ECS
// component
#include "component/transform/Transform.h"

using namespace OriGine;

PlayerFollowSystem::PlayerFollowSystem() : ISystem(SystemCategory::Movement) {}

void PlayerFollowSystem::Initialize() {}
void PlayerFollowSystem::Finalize() {}

void PlayerFollowSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    Transform* transform = GetComponent<Transform>(_handle);
    if (!transform) {
        LOG_ERROR("Transform component is not found.");
        return;
    }

    EntityHandle playerHandle  = GetUniqueEntity("Player");
    Transform* playerTransform = GetComponent<Transform>(playerHandle);
    if (!playerTransform) {
        LOG_ERROR("Player Transform component is not found.");
        return;
    }

    transform->parent = playerTransform;
    transform->UpdateMatrix();
}
