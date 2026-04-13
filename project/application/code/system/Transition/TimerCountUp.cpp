#include "TimerCountUp.h"

/// engine
#include "Engine.h"

/// ECS
// component
#include "component/TimerComponent.h"

/// math
#include "math/MathEnv.h"

using namespace OriGine;

TimerCountUp::TimerCountUp() : ISystem(SystemCategory::StateTransition) {}
TimerCountUp::~TimerCountUp() {}

void TimerCountUp::Initialize() {}
void TimerCountUp::Finalize() {}

void TimerCountUp::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& timerComponents = GetComponents<TimerComponent>(_handle);
    // タイマーコンポーネントがない場合は何もしない
    if (timerComponents.empty()) {
        return;
    }

    // タイマーコンポーネントの時間をカウントアップ
    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Timer");
    for (auto& timerComponent : timerComponents) {
        // 開始していないタイマーはスキップ
        if (!timerComponent.IsStarted()) {
            continue;
        }

        float currentTime = timerComponent.GetTime();
        timerComponent.SetCurrentTime(currentTime + deltaTime);
    }
}
