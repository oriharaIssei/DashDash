#include "ChangeSceneByButton.h"

/// engine
#define ENGINE_SCENE
#define ENGINE_ECS
#include "EngineInclude.h"

/// application
// component
#include "component/ui/Button.h"
#include "component/scene/SceneChanger.h"

ChangeSceneByButton::ChangeSceneByButton()
    : ISystem(OriGine::SystemCategory::StateTransition) {}

ChangeSceneByButton::~ChangeSceneByButton() {}

void ChangeSceneByButton::Initialize() {}

void ChangeSceneByButton::Finalize() {}

void ChangeSceneByButton::UpdateEntity(OriGine::EntityHandle _handle) {
    Button* button = GetComponent<Button>(_handle);

    if (button->IsReleased()) {
        SceneChanger* sceneChanger = GetComponent<SceneChanger>(_handle);
        sceneChanger->ChangeScene();
    }
}
