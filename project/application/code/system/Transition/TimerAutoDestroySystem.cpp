#include "TimerAutoDestroySystem.h"

/// engine
#include "scene/Scene.h"

/// component
#include "component/TimerComponent.h"

TimerAutoDestroySystem::TimerAutoDestroySystem() : ISystem(OriGine::SystemCategory::StateTransition) {}
TimerAutoDestroySystem::~TimerAutoDestroySystem() {}

void TimerAutoDestroySystem::Initialize() {}
void TimerAutoDestroySystem::Finalize() {}

void TimerAutoDestroySystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& timers = GetComponents<TimerComponent>(_handle);
    for (auto& timer : timers) {
        if (timer.IsStarted() && timer.GetTime() <= 0.0f) {
            GetScene()->AddDeleteEntity(_handle);
            return;
        }
    }
}
