#include "FollowTransformSystem.h"

/// engine
#include "Engine.h"

/// ECS
// component
#include "component/FollowTransformComponent.h"
#include "component/transform/Transform.h"

/// math
#include "math/Interpolation.h"

using namespace OriGine;

FollowTransformSystem::FollowTransformSystem() : ISystem(SystemCategory::Movement) {}

void FollowTransformSystem::Initialize() {}
void FollowTransformSystem::Finalize() {}

void FollowTransformSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto* follow = GetComponent<FollowTransformComponent>(_handle);
    if (!follow || !follow->IsActive()) {
        return;
    }

    Transform* transform = GetComponent<Transform>(_handle);
    if (!transform) {
        return;
    }

    Transform* targetTransform = GetComponent<Transform>(follow->GetTargetHandle());
    if (!targetTransform) {
        return;
    }

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");

    // 目標位置 = ターゲットのワールド座標 + オフセット
    Vec3f targetPos = targetTransform->GetWorldTranslate() + follow->GetOffset();

    // 感度に基づいた補間で追従
    transform->translate = LerpByDeltaTime(
        transform->translate,
        targetPos,
        deltaTime,
        follow->GetSensitivity());

    transform->UpdateMatrix();
}
