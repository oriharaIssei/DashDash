#include "ButtonScenePreviewSystem.h"

/// component
#include "component/scene/SubScene.h"
#include "component/ui/ButtonGroup.h"

using namespace OriGine;

ButtonScenePreviewSystem::ButtonScenePreviewSystem() : ISystem(OriGine::SystemCategory::StateTransition) {}
ButtonScenePreviewSystem::~ButtonScenePreviewSystem() {}

void ButtonScenePreviewSystem::Initialize() {}
void ButtonScenePreviewSystem::Finalize() {}

void ButtonScenePreviewSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto buttonGroup = GetComponent<ButtonGroup>(_handle);

    // skip
    if (!buttonGroup) {
        return;
    }

    // 選択されているOriGine::Sceneを描画
    int32_t currentButtonNumber = buttonGroup->GetCurrentButtonNumber();
    auto& buttonEntityHandles   = buttonGroup->GetButtonEntityHandles();
    int32_t index               = 0;
    for (auto& entityHandle : buttonEntityHandles) {
        // buttonのエンティティを取得
        OriGine::Entity* buttonEnt = GetEntity(entityHandle);
        if (!buttonEnt) {
            ++index;
            continue;
        }
        // ボタンが持つシーンを取得
        auto subScene = GetComponent<SubScene>(entityHandle);

        // なければスキップ
        if (!subScene) {
            ++index;
            continue;
        }

        subScene->SetActive(currentButtonNumber == index);
        ++index;
    }
}
