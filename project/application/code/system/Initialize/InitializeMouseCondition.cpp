#include "InitializeMouseCondition.h"

/// engine
#include "input/MouseInput.h"
#include "scene/Scene.h"

/// component
#include "component/MouseCondition.h"

using namespace OriGine;

InitializeMouseCondition::InitializeMouseCondition() : ISystem(OriGine::SystemCategory::Initialize) {}

void InitializeMouseCondition::Initialize() {}
void InitializeMouseCondition::Finalize() {}

void InitializeMouseCondition::UpdateEntity(OriGine::EntityHandle _handle) {
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
