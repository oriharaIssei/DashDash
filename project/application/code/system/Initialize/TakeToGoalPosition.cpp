#include "TakeToGoalPosition.h"

/// ECS
// component
#include "component/material/light/PointLight.h"
#include "component/transform/Transform.h"

using namespace OriGine;

TakeToGoalPosition::TakeToGoalPosition() : ISystem(SystemCategory::Initialize) {}
TakeToGoalPosition::~TakeToGoalPosition() {}

void TakeToGoalPosition::Initialize() {}
void TakeToGoalPosition::Finalize() {}

void TakeToGoalPosition::UpdateEntity(EntityHandle _handle) {
    Transform* transform = GetComponent<Transform>(_handle);

    EntityHandle goalPositionEntityHandle = GetUniqueEntity("GoalPosition");
    Transform* goalTransform              = GetComponent<OriGine::Transform>(goalPositionEntityHandle);

    Vec3f goalPos = {};
    if (goalTransform) {
        goalPos = goalTransform->GetWorldTranslate();
    }
    transform->translate = goalPos;
    transform->UpdateMatrix();

    auto& pointLights = GetComponents<PointLight>(_handle);
    for (auto& pointLight : pointLights) {
        pointLight.pos = goalPos;
    }
}
