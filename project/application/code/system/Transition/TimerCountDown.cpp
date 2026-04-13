#include "TimerCountDown.h"

/// engine
#include "Engine.h"

/// ECS
// component
#include "component/TimerComponent.h"

using namespace OriGine;

TimerCountDown::TimerCountDown() : ISystem(SystemCategory::StateTransition) {}

TimerCountDown::~TimerCountDown() {}

void TimerCountDown::Initialize() {}

void TimerCountDown::Finalize() {}

void TimerCountDown::UpdateEntity(EntityHandle _handle) {
    auto& timerComponents = GetComponents<TimerComponent>(_handle);
    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Timer");
    for (auto& timerComponent : timerComponents) {
        if (timerComponent.IsStarted()) {
            timerComponent.SetCurrentTime(timerComponent.GetTime() - deltaTime);
        }
    }
}
