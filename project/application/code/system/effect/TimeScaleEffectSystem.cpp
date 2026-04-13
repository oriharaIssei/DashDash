#include "TimeScaleEffectSystem.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"

/// ECS
// component
#include "component/effect/post/GrayscaleComponent.h"
#include "component/gimmick/TimeScaleEffectComponent.h"
// system
#include "system/postRender/GrayScaleEffect.h"

using namespace OriGine;

TimeScaleEffectSystem::TimeScaleEffectSystem() : ISystem(SystemCategory::Effect) {}
TimeScaleEffectSystem::~TimeScaleEffectSystem() {}

void TimeScaleEffectSystem::Initialize() {
    DeltaTimer* deltaTimer = Engine::GetInstance()->GetDeltaTimer();

    deltaTimer->SetTimeScale("Player", 1.f);
    deltaTimer->SetTimeScale("Effect", 1.f);
    deltaTimer->SetTimeScale("Camera", 1.f);
    deltaTimer->SetTimeScale("Timer", 1.f);

    // GrayScale Effect Entityの作成
    grayScaleEffectEntityHandle_  = CreateEntity("GrayscaleEffect", false);
    Entity* grayScaleEffectEntity = GetEntity(grayScaleEffectEntityHandle_);
    grayScaleEffectEntity->SetShouldSave(false); // セーブ対象外
    AddComponent<GrayscaleComponent>(grayScaleEffectEntityHandle_);

    GetScene()->GetSystem(nameof<GrayscaleEffect>())->AddEntity(grayScaleEffectEntityHandle_);
}
void TimeScaleEffectSystem::Finalize() {}

void TimeScaleEffectSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto* timeScaleEffect = GetComponent<TimeScaleEffectComponent>(_handle);
    if (!timeScaleEffect || !timeScaleEffect->IsActive()) {
        return;
    }

    // タイムスケールの更新
    for (auto& key : timeScaleEffect->GetTimeScaleTags()) {
        stopTimesByTimeScale_[key].timeScale = timeScaleEffect->GetTimeScale();
        stopTimesByTimeScale_[key].scaleTimer += timeScaleEffect->GetTimeScaleDuration();
    }

    // エンティティの削除
    GetScene()->AddDeleteEntity(_handle);
}

void TimeScaleEffectSystem::Update() {
    if (!entities_.empty()) {
        EraseDeadEntity();

        for (auto& entityID : entities_) {
            UpdateEntity(entityID);
        }
    }

    // タイムスケールタイマーの更新
    UpdateScaleTimer();

    // fade In/Out の更新
    float leftTime = 0.f;
    for (const auto& [key, timeScaleComponent] : stopTimesByTimeScale_) {
        if (timeScaleComponent.scaleTimer > 0.f) {
            leftTime = (std::max)(leftTime, timeScaleComponent.scaleTimer);
        }
    }

    // 有効なタイムスケールが存在する場合はグレースケールエフェクトを有効化、存在しない場合は無効化
    DeltaTimer* deltaTimer = Engine::GetInstance()->GetDeltaTimer();
    if (leftTime > 0.f) {
        effectElapsedTime_ += deltaTimer->GetDeltaTime();
    } else {
        effectElapsedTime_ = 0.f;
    }

    float fadeInRatio = 1.f;
    if (fadeInTime_ > 0.f) {
        fadeInRatio = std::clamp(effectElapsedTime_ / fadeInTime_, 0.f, 1.f);
    }

    float fadeOutRatio = 0.f;
    if (leftTime > 0.f) {
        if (fadeOutTime_ > 0.f) {
            fadeOutRatio = std::clamp(leftTime / fadeOutTime_, 0.f, 1.f);
        } else {
            fadeOutRatio = 1.f;
        }
    }

    float effectAmount = (std::min)(fadeInRatio, fadeOutRatio);

    GrayscaleComponent* grayScaleEffect = GetComponent<GrayscaleComponent>(grayScaleEffectEntityHandle_);
    if (grayScaleEffect) {
        grayScaleEffect->SetIsEnabled(leftTime > 0.f);
        grayScaleEffect->SetAmount(effectAmount);
    } else {
        LOG_ERROR("GrayscaleComponent is not found.");
    }
}

void TimeScaleEffectSystem::UpdateScaleTimer() {
    // タイムスケールの適用
    DeltaTimer* deltaTimer = Engine::GetInstance()->GetDeltaTimer();
    for (const auto& [key, timeScaleComponent] : stopTimesByTimeScale_) {
        if (timeScaleComponent.scaleTimer > 0.f) {
            deltaTimer->SetTimeScale(key, timeScaleComponent.timeScale);
            // 経過時間を減らす
            stopTimesByTimeScale_[key].scaleTimer -= deltaTimer->GetDeltaTime();
        } else {
            // タイムスケールのリセット
            stopTimesByTimeScale_[key].scaleTimer = 0.f;
            deltaTimer->SetTimeScale(key, 1.0f);
        }
    }
}
