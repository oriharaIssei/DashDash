#include "TimeStopEffect.h"

/// Engine
#include "Engine.h"

/// ECS
// component
#include "component/renderer/Sprite.h"
#include "component/TimerForSpriteComponent.h"

/// Util
#include "deltaTime/DeltaTimer.h"

using namespace OriGine;

TimeStopEffect::TimeStopEffect() : ISystem(SystemCategory::Effect) {}

void TimeStopEffect::Initialize() {}
void TimeStopEffect::Finalize() {}

void TimeStopEffect::Update() {
    constexpr Vec4f kDefaultColor   = Vec4f(1.f, 1.f, 1.f, 1.f);
    constexpr Vec4f kStopColor      = Vec4f(0.134f, 0.134f, 0.134f, 1.f);
    constexpr float kDefTimerScale  = 1.f;
    constexpr char kTimerScaleKey[] = "Timer";

    // Timer用の DeltaTimeScale が デフォルトでない場合、時間を停止しているとして、処理を行う
    bool isStop                   = false;
    DeltaTimer* deltaTimer        = Engine::GetInstance()->GetDeltaTimer();
    const auto& deltaTimeScaleMap = deltaTimer->GetDeltaTimeScaleMap();
    auto timerScaleItr            = deltaTimeScaleMap.find(kTimerScaleKey);
    if (timerScaleItr != deltaTimeScaleMap.end()) {
        // Scaleが デフォルト以下なら、時間停止しているとみなす
        if (timerScaleItr->second < kDefTimerScale) {
            isStop = true;
        }
    }

    EntityHandle timerEntityHandle = GetUniqueEntity("Timer");
    if (!timerEntityHandle.IsValid()) {
        return;
    }
    TimerForSpriteComponent* timerForSprite = GetComponent<TimerForSpriteComponent>(timerEntityHandle);

    auto& sprites = GetComponents<SpriteRenderer>(timerForSprite->GetSpritesEntityHandle());
    for (auto& sprite : sprites) {
        sprite.GetSpriteBuff()->color_ = isStop ? kStopColor : kDefaultColor;
    }
}
