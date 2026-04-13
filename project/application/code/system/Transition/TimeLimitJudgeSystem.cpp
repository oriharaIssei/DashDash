#include "TimeLimitJudgeSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// event
#include "event/GamefailedEvent.h"

/// ECS
// component
#include "component/scene/SceneChanger.h"
#include "component/TimerComponent.h"

/// math
#include "math/MathEnv.h"

using namespace OriGine;

TimeLimitJudgeSystem::TimeLimitJudgeSystem() : ISystem(SystemCategory::StateTransition) {}
TimeLimitJudgeSystem::~TimeLimitJudgeSystem() {}

void TimeLimitJudgeSystem::Initialize() {}
void TimeLimitJudgeSystem::Finalize() {}

void TimeLimitJudgeSystem::UpdateEntity(EntityHandle _handle) {
    auto timerComp = GetComponent<TimerComponent>(_handle);
    if (!timerComp) {
        return;
    }
    if (timerComp->GetTime() <= -kEpsilon) {
        MessageBus::GetInstance()->Emit<GamefailedEvent>(GamefailedEvent());
        return;
    }
}
