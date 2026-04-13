#include "SelectPreviewSceneInitialize.h"

/// ECS
// component
#include "component/scene/SubScene.h"

// system
#include "system/SystemRunner.h"

#include "system/render/OverlayRenderSystem.h"
#include "system/render/SpriteRenderSystem.h"

using namespace OriGine;

SelectPreviewSceneInitialize::SelectPreviewSceneInitialize() : ISystem(SystemCategory::Initialize) {}
SelectPreviewSceneInitialize::~SelectPreviewSceneInitialize() {}

void SelectPreviewSceneInitialize::Initialize() {}
void SelectPreviewSceneInitialize::Finalize() {}

void SelectPreviewSceneInitialize::UpdateEntity(EntityHandle _handle) {
    auto& subScenes = GetComponents<SubScene>(_handle);

    if (subScenes.empty()) {
        return;
    }

    for (auto& subScene : subScenes) {
        if (!subScene.IsActive()) {
            const std::string& sceneName = subScene.GetSceneName();
            if (sceneName.empty()) {
                continue;
            }
            subScene.Load(sceneName);
        }

        auto subSceneRef = subScene.GetSubSceneRef();

        auto* systemRunnerRef = subSceneRef->GetSystemRunnerRef();
        // カテゴリを 絞る
        // Effect と Render と PostRender だけ
        systemRunnerRef->SetCategoryActivity(SystemCategory::Input, false);
        systemRunnerRef->SetCategoryActivity(SystemCategory::StateTransition, false);
        systemRunnerRef->SetCategoryActivity(SystemCategory::Movement, false);
        systemRunnerRef->SetCategoryActivity(SystemCategory::Collision, false);
        systemRunnerRef->SetCategoryActivity(SystemCategory::PostRender, false);

        // uiの表示を切る
        systemRunnerRef->DeactivateSystem(nameof<OverlayRenderSystem>());
        systemRunnerRef->DeactivateSystem(nameof<SpriteRenderSystem>());
    }
}
