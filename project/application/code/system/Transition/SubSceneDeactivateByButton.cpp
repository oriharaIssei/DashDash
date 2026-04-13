#include "SubSceneDeactivateByButton.h"

/// engine
#include "scene/Scene.h"

/// component
#include "component/ui/Button.h"

SubSceneDeactivateByButton::SubSceneDeactivateByButton() : ISystem(OriGine::SystemCategory::StateTransition) {}
SubSceneDeactivateByButton::~SubSceneDeactivateByButton() {}

void SubSceneDeactivateByButton::Initialize() {}
void SubSceneDeactivateByButton::Finalize() {}

void SubSceneDeactivateByButton::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& buttons = GetComponents<Button>(_handle);
    if (buttons.empty()) {
        return;
    }

    for (auto& button : buttons) {
        if (button.IsReleased()) {
            GetScene()->SetActive(false);
            return;
        }
    }
}
