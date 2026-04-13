#include "PenaltySystem.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"
#include "winApp/WinApp.h"

/// component
#include "component/animation/MaterialAnimation.h"
#include "component/animation/SpriteAnimation.h"
#include "component/player/state/PlayerState.h"
#include "component/renderer/Sprite.h"
#include "component/TimerComponent.h"
#include "component/TimerForSpriteComponent.h"

/// system
#include "system/SystemRunner.h"

#include "system/effect/PenaltyTimeSpriteUpdate.h"
#include "system/effect/SpriteAnimationSystem.h"
#include "system/effect/TimerForSprite.h"

/// math
#include "math/MathEnv.h"

/// config
#include "component/player/PlayerConfig.h"

using namespace OriGine;

PenaltySystem::PenaltySystem() : ISystem(OriGine::SystemCategory::StateTransition) {}
PenaltySystem::~PenaltySystem() {}

void PenaltySystem::Initialize() {
    createSpriteFromTimerSystem_ = std::make_unique<CreateSpriteFromTimer>();
    createSpriteFromTimerSystem_->SetScene(GetScene());
    createSpriteFromTimerSystem_->Initialize();
}
void PenaltySystem::Finalize() {
    createSpriteFromTimerSystem_->Finalize();
    createSpriteFromTimerSystem_.reset();
}

void PenaltySystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto* playerState = GetComponent<PlayerState>(_handle);

    // ペナルティ状態でなければ 処理しない
    if (!playerState->IsPenalty()) {
        return;
    };

    // ペナルティ状態を解除
    // マイナスする時間を取得
    float penaltyTime = playerState->SufferPenalty();

    // ギアレベルを下げる
    int32_t newGearLevel = playerState->GetGearLevel();
    newGearLevel         = (std::max)((std::min)(newGearLevel, AppConfig::Player::kPenaltyThresholdGearLevel), newGearLevel / AppConfig::Player::kPenaltyGearLevelDivisor);
    playerState->SetGearLevel(newGearLevel);

    playerState->GetStateFlagRef().CurrentRef().SetFlag(PlayerStateFlag::GEAR_UP);

    // Playerが操作しているエンティティでなければ 処理しない
    if (_handle != GetUniqueEntity("Player")) {
        return;
    }

    // TimerComponent を取得 & ペナルティー分 時間をマイナス
    EntityHandle tierEntity = GetUniqueEntity("Timer");
    TimerComponent* timer   = GetComponent<TimerComponent>(tierEntity);
    if (!timer) {
        LOG_ERROR("TimerComponent not found");
        return;
    }

    timer->SetCurrentTime(timer->GetTime() + penaltyTime);

    /// ペナルティー時間を表示する

    EntityHandle penaltyTimeUIEntityHandle = CreateEntity("PenaltyTimeUI");
    // Penaltyを表す時間
    TimerComponent penaltyTimer = TimerComponent();
    penaltyTimer.SetMaxTime(penaltyTime);
    penaltyTimer.SetCurrentTime(penaltyTime);

    // 情報だけ持つ事前コンポーネントから情報をコピー
    EntityHandle forSpriteDataEntityHandle   = GetUniqueEntity("PenaltyTimerForSprite");
    TimerForSpriteComponent* forSpriteData   = GetComponent<TimerForSpriteComponent>(forSpriteDataEntityHandle);
    TimerForSpriteComponent timer4SpriteComp = *forSpriteData;
    timer4SpriteComp.SetSpritesEntityHandle(penaltyTimeUIEntityHandle);

    // 桁数を設定
    timer4SpriteComp.digitInteger = (std::max)(CountIntegralDigits<float, int>(penaltyTime), timer4SpriteComp.digitInteger);
    timer4SpriteComp.digitDecimal = (std::max)(CountDecimalDigits<float, int>(penaltyTime), timer4SpriteComp.digitDecimal);

    /// コンポーネントを追加
    ComponentHandle timerHandle = AddComponent<TimerComponent>(penaltyTimeUIEntityHandle);
    TimerComponent* timerComp   = GetComponent<TimerComponent>(penaltyTimeUIEntityHandle);
    *timerComp                  = penaltyTimer;
    timerComp->SetHandle(timerHandle); // ハンドルを戻す

    ComponentHandle timer4SpriteHandle           = AddComponent<TimerForSpriteComponent>(penaltyTimeUIEntityHandle);
    TimerForSpriteComponent* timer4SpriteCompPtr = GetComponent<TimerForSpriteComponent>(penaltyTimeUIEntityHandle);
    *timer4SpriteCompPtr                         = timer4SpriteComp;
    timer4SpriteCompPtr->SetHandle(timer4SpriteHandle); // ハンドルを戻す

    /// スプライトを作成
    createSpriteFromTimerSystem_->CreateSprites(penaltyTimeUIEntityHandle, timer4SpriteCompPtr);
    // animationを設定
    auto& sprites = GetComponents<SpriteRenderer>(penaltyTimeUIEntityHandle);
    if (sprites.empty()) {
        return;
    }

    SpriteAnimation animation = *GetComponent<SpriteAnimation>(forSpriteDataEntityHandle);
    animation.PlayStart();
    for (auto& sprite : sprites) {
        auto animationComp     = GetComponent<SpriteAnimation>(AddComponent<SpriteAnimation>(penaltyTimeUIEntityHandle));
        ComponentHandle handle = animationComp->GetHandle();
        *animationComp         = animation;
        animationComp->SetHandle(handle); // ハンドルを戻す
        animationComp->SetSpriteComponentHandle(sprite.GetHandle());

        // それぞれの位置からアニメーションするように
        for (auto& translate : animationComp->GetTranslateCurve()) {
            translate.value += sprite.GetTranslate();
        }
    }

    { /// system に登録
        auto systemRunner = GetScene()->GetSystemRunnerRef();
        systemRunner->RegisterEntity<PenaltyTimeSpriteUpdate>(penaltyTimeUIEntityHandle);
        systemRunner->RegisterEntity<TimerForSprite>(penaltyTimeUIEntityHandle);
        systemRunner->RegisterEntity<SpriteAnimationSystem>(penaltyTimeUIEntityHandle);
    }
}
