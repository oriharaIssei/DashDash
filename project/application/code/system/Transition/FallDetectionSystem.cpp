#include "FallDetectionSystem.h"

/// ECS
// component
#include "component/Transform/Transform.h"

/// config
#include "component/player/PlayerConfig.h"

using namespace OriGine;

FallDetectionSystem::FallDetectionSystem() : ISystem(OriGine::SystemCategory::StateTransition) {}
FallDetectionSystem::~FallDetectionSystem() {}

void FallDetectionSystem::Update() {
    EntityHandle playerEntity = GetUniqueEntity("Player");
    if (!playerEntity.IsValid()) {
        return;
    }
    Transform* transform = GetComponent<Transform>(playerEntity);

    if (!transform) {
        return;
    }

    if (transform->worldMat[3][Y] < AppConfig::Player::kFallThresholdY) {
        OnFall();
    }
}
