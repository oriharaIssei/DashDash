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

void TimeLimitJudgeSystem::UpdateEntity(const EntityHandle& _handle) {
    auto timerComp = GetComponent<TimerComponent>(_handle);
    if (!timerComp) {
        return;
    }
    // 残り時間が0を下回った(浮動小数点誤差を考慮しkEpsilon分のマージンを持たせる)らゲーム失敗
    if (timerComp->GetTime() <= -kEpsilon) {
        MessageBus::GetInstance()->Emit<GamefailedEvent>(GamefailedEvent());
        return;
    }
}
