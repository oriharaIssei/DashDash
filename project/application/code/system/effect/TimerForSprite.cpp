#include "TimerForSprite.h"

/// engine
#include "Engine.h"
#include "winApp/WinApp.h"

/// ECS
// component
#include "component/renderer/Sprite.h"
#include "component/TimerComponent.h"
#include "component/TimerForSpriteComponent.h"

/// math
#include "math/MathEnv.h"

using namespace OriGine;

TimerForSprite::TimerForSprite()
    : ISystem(OriGine::SystemCategory::Effect) {}
TimerForSprite::~TimerForSprite() {}

void TimerForSprite::Initialize() {}
void TimerForSprite::Finalize() {}

void TimerForSprite::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& timerComponents          = GetComponents<TimerComponent>(_handle);
    auto& timerForSpriteComponents = GetComponents<TimerForSpriteComponent>(_handle);
    if (timerComponents.empty() || timerForSpriteComponents.empty()) {
        return; // タイマーコンポーネントがない場合は何もしない
    }

    // TimerとTimerForSpriteは1対1で存在する想定だが、万が一数が異なる場合は少ない方の数でループする
    size_t minSize = std::min(timerComponents.size(), timerForSpriteComponents.size());
    for (size_t i = 0; i < minSize; ++i) {
        auto& timerComp          = timerComponents[i];
        auto& timerForSpriteComp = timerForSpriteComponents[i];

        auto timerSpritesEntity = GetEntity(timerForSpriteComp.GetSpritesEntityHandle());
        if (!timerSpritesEntity) {
            continue; // スプライトエンティティがない場合は何もしない
        }

        // 浮動小数点数から各桁の数字を抽出
        std::vector<int> digits = CalculateDigitsFromFloat(
            timerComp.GetTime(),
            timerForSpriteComp.digitInteger,
            timerForSpriteComp.digitDecimal);

        OriGine::Vec2f windowSize = Engine::GetInstance()->GetWinApp()->GetWindowSize();

        // 各スプライトに数字を適用
        for (int32_t j = 0; j < timerForSpriteComp.digitInteger + timerForSpriteComp.digitDecimal; ++j) {
            if (int32_t(digits.size()) <= j) {
                break;
            }

            auto sprite = GetComponent<SpriteRenderer>(timerSpritesEntity->GetHandle(), j);
            if (!sprite) {
                continue; // スプライトがない場合は何もしない
            }
            float spriteLeftTopY     = sprite->GetTextureLeftTop()[Y];
            float spriteTextureSizeX = sprite->GetTextureSize()[X];

            sprite->SetTextureLeftTop(
                {spriteTextureSizeX * digits[j], spriteLeftTopY});

            sprite->CalculatePosRatioAndSizeRatio(windowSize);
        }
    }
}
