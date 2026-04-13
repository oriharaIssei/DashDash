#include "RestartOnButtonPressSystem.h"

/// Engine
#include "messageBus/MessageBus.h"

/// ECS
// component
#include "component/ui/Button.h"

/// Event
#include "event/RestartEvent.h"

using namespace OriGine;

RestartOnButtonPressSystem::RestartOnButtonPressSystem() : ISystem(SystemCategory::StateTransition) {}
RestartOnButtonPressSystem::~RestartOnButtonPressSystem() {}

void RestartOnButtonPressSystem::Initialize() {}
void RestartOnButtonPressSystem::Finalize() {}

void RestartOnButtonPressSystem::UpdateEntity(EntityHandle _handle) {
    // リスタートボタンが押されたらRestartEventを飛ばす
    auto& buttonComps = GetComponents<Button>(_handle);
    if (buttonComps.empty()) {
        return;
    }

    for (auto& button : buttonComps) {
        if (button.IsReleased()) {
            // リスタートイベントを飛ばす
            MessageBus::GetInstance()->Emit<RestartEvent>(RestartEvent());
            return;
        }
    }
}
