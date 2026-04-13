#include "PenaltyTimeSpriteUpdate.h"

/// engine
#include "scene/Scene.h"

/// component
#include "component/animation/SpriteAnimation.h"
#include "component/TimerComponent.h"

/// math
#include "math/MyEasing.h"

using namespace OriGine;

PenaltyTimeSpriteUpdate::PenaltyTimeSpriteUpdate() : ISystem(SystemCategory::Effect) {}
PenaltyTimeSpriteUpdate::~PenaltyTimeSpriteUpdate() {}

void PenaltyTimeSpriteUpdate::Initialize() {}
void PenaltyTimeSpriteUpdate::Finalize() {}

void PenaltyTimeSpriteUpdate::UpdateEntity(EntityHandle _handle) {
    auto timerComp = GetComponent<TimerComponent>(_handle);

    // TimerComponentが存在しない場合は処理しない
    if (!timerComp) {
        return;
    }
    // SpriteAnimationが存在しない場合は処理しない
    auto spriteAnimation = GetComponent<SpriteAnimation>(_handle);
    if (!spriteAnimation) {
        return;
    }

    // アニメーション終了したらエンティティを削除
    if (spriteAnimation->IsEnded()) {
        GetScene()->AddDeleteEntity(_handle);
    };
}
