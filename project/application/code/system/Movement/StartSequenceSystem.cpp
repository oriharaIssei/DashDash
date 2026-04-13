#include "StartSequenceSystem.h"

/// engine
#include "scene/Scene.h"

/// component
#include "component/animation/SpriteAnimation.h"
#include "component/effect/CameraAction.h"
#include "component/renderer/Sprite.h"
#include "component/TimerForSpriteComponent.h"
#include "component/transform/CameraTransform.h"
#include "component/ui/Button.h"

/// system
#include "system/SystemRunner.h"

#include "system/Input/PlayerInputSystem.h"
#include "system/Movement/PlayerMoveSystem.h"
#include "system/Transition/PenaltySystem.h"

#include "system/Input/CameraInputSystem.h"
#include "system/Movement/FollowCameraUpdateSystem.h"

/// util
#include "nameof.h"

using namespace OriGine;

StartSequenceSystem::StartSequenceSystem() : ISystem(OriGine::SystemCategory::Movement) {}
StartSequenceSystem::~StartSequenceSystem() {}

void StartSequenceSystem::Initialize() {
    stopSystemsInStartTimer_ = {
        nameof<CameraInputSystem>(),
        nameof<FollowCameraUpdateSystem>(),
    };

    stopSystems_ = {
        nameof<PlayerMoveSystem>(),
        nameof<PlayerInputSystem>(),
        nameof<PenaltySystem>()};

    isInitializeState_ = false;

#ifndef _DEBUG
    StageIntroductionSequence();
#endif // ! _DEBUG
}

void StartSequenceSystem::Finalize() {}

void StartSequenceSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    if (!isInitializeState_) {
        // ステージ紹介シーケンスを開始する
        ChangeState(stateType_, _handle);
        isInitializeState_ = true;
    }
    if (isStarted_ || !currentState_) {
        return;
    }

    StopGameTimer();
    currentState_->Update(*this, _handle);
}

void StartSequenceSystem::StageIntroductionSequence() {
    // 特定のシステムを停止する
    auto* systemRunner = GetScene()->GetSystemRunnerRef();

    for (const auto& systemName : stopSystemsInStartTimer_) {
        systemRunner->DeactivateSystem(systemName);
    }

    for (const auto& systemName : stopSystems_) {
        systemRunner->DeactivateSystem(systemName);
    }
}

void StartSequenceSystem::TimerStartSequence() {
    // 特定のシステムを停止する
    auto* systemRunner = GetScene()->GetSystemRunnerRef();

    // スタートタイマー中に動かすシステムを再開する
    for (const auto& systemName : stopSystemsInStartTimer_) {
        systemRunner->ActivateSystem(systemName);
    }
}

void StartSequenceSystem::GameStartSequence() {
    // 停止していたシステムを再開する
    auto* systemRunner = GetScene()->GetSystemRunnerRef();
    for (const auto& systemName : stopSystems_) {
        systemRunner->RegisterSystem(systemName);
    }
}

void StartSequenceSystem::ChangeState(StartSequenceStateType _type, OriGine::EntityHandle _handle) {
    stateType_ = _type;

    switch (_type) {
    case StartSequenceStateType::Introduction:
        currentState_ = std::make_unique<IntroductionState>();
        break;
    case StartSequenceStateType::StartTimer:
        currentState_ = std::make_unique<StartTimerState>();
        break;
    case StartSequenceStateType::GameStart:
        currentState_ = std::make_unique<GameStartState>();
        break;
    }

    currentState_->Enter(*this, _handle);
}

void StartSequenceSystem::StopGameTimer() {
    EntityHandle timerEntity = GetUniqueEntity("Timer");
    if (auto* timer = GetComponent<TimerComponent>(timerEntity)) {
        timer->SetStarted(false);
    }
}

void StartSequenceSystem::DisableGameCamera() {
    EntityHandle gameCamera = GetUniqueEntity("GameCamera");
    if (auto* cam = GetComponent<CameraTransform>(gameCamera)) {
        cam->canUseMainCamera = false;
    }
}

void StartSequenceSystem::EnableGameCamera() {
    EntityHandle gameCamera = GetUniqueEntity("GameCamera");
    if (auto* cam = GetComponent<CameraTransform>(gameCamera)) {
        cam->canUseMainCamera = true;
    }

    EntityHandle introCamera = GetUniqueEntity("StageIntroCamera");
    if (auto* cam = GetComponent<CameraTransform>(introCamera)) {
        cam->canUseMainCamera = false;
    }
}

bool StartSequenceSystem::IsIntroductionCameraEnd() {
    EntityHandle introCamera = GetUniqueEntity("StageIntroCamera");
    if (auto* action = GetComponent<CameraAction>(introCamera)) {
        action->SetIsLoop(false);
        return action->IsEnd();
    }
    return false;
}

void StartSequenceSystem::SetStartSpriteVisible(OriGine::EntityHandle _handle, bool _visible) {
    auto* timer4Sprite = GetComponent<TimerForSpriteComponent>(_handle);
    if (!timer4Sprite) {
        return;
    }

    Entity* spriteEntity = GetEntity(timer4Sprite->GetSpritesEntityHandle());
    if (!spriteEntity) {
        return;
    }

    auto& sprites = GetComponents<SpriteRenderer>(spriteEntity->GetHandle());
    for (auto& sprite : sprites) {
        sprite.SetIsRender(_visible);
    }
    // trueならAnimationも再生する
    if (_visible) {
        auto& spriteAnimations = GetComponents<SpriteAnimation>(spriteEntity->GetHandle());
        for (auto& spriteAnimation : spriteAnimations) {
            spriteAnimation.PlayColorAnimation();
            spriteAnimation.PlayTransformAnimation();
            spriteAnimation.PlayUVAnimation();
        }

    }
}

void StartSequenceSystem::CleanupEntities(OriGine::EntityHandle _handle) {
    GetScene()->AddDeleteEntity(_handle);

    if (auto* timer4Sprite = GetComponent<TimerForSpriteComponent>(_handle)) {
        GetScene()->AddDeleteEntity(timer4Sprite->GetSpritesEntityHandle());
    }
}

void StartSequenceSystem::IntroductionState::Enter(StartSequenceSystem& _system, OriGine::EntityHandle _handle) {
    _system.DisableGameCamera();
    _system.SetStartSpriteVisible(_handle, false);
}

void StartSequenceSystem::IntroductionState::Update(StartSequenceSystem& _system, OriGine::EntityHandle _handle) {
    auto* timer = _system.GetComponent<TimerComponent>(_handle);
    if (!timer) {
        return;
    }

    timer->SetStarted(false);

    bool isEnd = _system.IsIntroductionCameraEnd();
    if (!isEnd) {
        const auto& skipButtons = _system.GetComponents<Button>(_handle);
        for (auto& skipButton : skipButtons) {
            isEnd |= skipButton.IsReleased();
        }
    }

    if (!isEnd) {
        return;
    }

    _system.SetStartSpriteVisible(_handle, true);
    _system.EnableGameCamera();
    _system.TimerStartSequence();

    timer->SetStarted(true);
    _system.ChangeState(StartSequenceStateType::StartTimer, _handle);
}

void StartSequenceSystem::StartTimerState::Update(StartSequenceSystem& system, OriGine::EntityHandle _handle) {
    auto* timer = system.GetComponent<TimerComponent>(_handle);
    if (!timer || timer->GetTime() > 0.f) {
        return;
    }

    system.GameStartSequence();
    system.ChangeState(StartSequenceStateType::GameStart, _handle);
}

void StartSequenceSystem::GameStartState::Enter(StartSequenceSystem& _system, OriGine::EntityHandle _handle) {
    if (auto* gameTimer = _system.GetComponent<TimerComponent>(
            _system.GetUniqueEntity("Timer"))) {
        gameTimer->SetStarted(true);
    }

    _system.CleanupEntities(_handle);
    _system.isStarted_ = true;
}
