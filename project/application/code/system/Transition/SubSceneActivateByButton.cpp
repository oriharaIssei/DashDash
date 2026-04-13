#include "SubSceneActivateByButton.h"

// component
#include "component/scene/SubScene.h"
#include "component/ui/Button.h"

using namespace OriGine;

SubSceneActivateByButton::SubSceneActivateByButton() : ISystem(OriGine::SystemCategory::StateTransition) {}
SubSceneActivateByButton::~SubSceneActivateByButton() {}

void SubSceneActivateByButton::Initialize() {}

void SubSceneActivateByButton::Finalize() {}

void SubSceneActivateByButton::UpdateEntity(EntityHandle _handle) {
    auto* subScene = GetComponent<SubScene>(_handle);
    if (!subScene) {
        return;
    }

    // 多重初期化防止
    if (subScene->IsActive()) {
        return;
    }

    auto& buttons = GetComponents<Button>(_handle);
    if (buttons.empty()) {
        return;
    }

    for (auto& button : buttons) {
        // ボタンが押されたらシーンをアクティブにする
        if (button.IsReleased()) {
            subScene->Activate();
            return;
        }
    }
}
