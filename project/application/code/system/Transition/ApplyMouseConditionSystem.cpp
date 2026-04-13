#include "ApplyMouseConditionSystem.h"

/// engine
#include "scene/Scene.h"
// input
#include "input/MouseInput.h"

/// component
#include "component/MouseCondition.h"

using namespace OriGine;

ApplyMouseConditionSystem::ApplyMouseConditionSystem() : ISystem(OriGine::SystemCategory::StateTransition) {}
ApplyMouseConditionSystem::~ApplyMouseConditionSystem() = default;

void ApplyMouseConditionSystem::Initialize() {}
void ApplyMouseConditionSystem::Finalize() {}

void ApplyMouseConditionSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& conditions = GetComponents<MouseCondition>(_handle);
    if (conditions.empty()) {
        return;
    }

    MouseInput* mouseInput = GetScene()->GetMouseInput();

    for (auto& condition : conditions) {
        if (!condition.IsActive()) {
            continue;
        }

        mouseInput->ShowCursor(condition.IsShowCursor());

        if (condition.IsFixCursor()) {
            // マウスの座標を指定し続けることで、実質的にマウスを固定する
            // このシステムを使用して Mouse座標を固定することで Velocityを取得しながら Mouse座標を固定できる
            mouseInput->SetPosition(condition.GetFixCursorPos());
        }
    }
}
