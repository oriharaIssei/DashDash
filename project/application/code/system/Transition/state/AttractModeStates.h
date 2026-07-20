#pragma once

#include "IAttractModeState.h"

/// engine
#include "scene/Scene.h"

class AttractIdleState;
class AttractActiveState;

/// <summary>
/// シーン初期化中の状態
/// </summary>
class AttractInitializingState : public IAttractModeState {
public:
    AttractInitializingState(float _delay, AttractActiveState* _activeState)
        : delay_(_delay)
        , activeState_(_activeState) {}

    void Enter() override;
    void Update(float _deltaTime) override;

    bool IsActive() const override { return false; }
    bool ShouldRender() const override { return true; }
    IAttractModeState* Transition() override;

private:
    // Active状態へ遷移するまでの初期化待機時間
    float delay_ = 1.0f;
    // 初期化開始からの経過時間
    float timer_ = 0.0f;
    // 遷移先のActive状態
    AttractActiveState* activeState_ = nullptr;
};

/// <summary>
/// アトラクションモード有効状態 (入力待ち)
/// </summary>
class AttractActiveState : public IAttractModeState {
public:
    AttractActiveState(OriGine::Scene* _scene, AttractIdleState* _idleState)
        : scene_(_scene)
        , idleState_(_idleState) {}

    void Update(float _deltaTime) override;

    bool IsActive() const override { return true; }
    bool ShouldRender() const override { return true; }
    IAttractModeState* Transition() override;

private:
    OriGine::Scene* scene_        = nullptr;
    // 直近のUpdateで何らかの入力を検知したか
    bool hasInput_                = false;
    // 遷移先のIdle状態
    AttractIdleState* idleState_  = nullptr;
};

/// <summary>
/// アイドル状態 (未入力時間を計測し、閾値を超えたらActiveへ遷移)
/// </summary>
class AttractIdleState : public IAttractModeState {
public:
    AttractIdleState(OriGine::Scene* _scene, float _threshold, AttractActiveState* _activeState)
        : scene_(_scene)
        , threshold_(_threshold)
        , activeState_(_activeState) {}

    void Enter() override;
    void Update(float _deltaTime) override;

    bool IsActive() const override { return false; }
    bool ShouldRender() const override { return false; }
    IAttractModeState* Transition() override;

private:
    OriGine::Scene* scene_            = nullptr;
    // Active状態へ遷移する未入力時間の閾値(秒)
    float threshold_                  = 30.0f;
    // 未入力状態が続いている時間
    float idleTimer_                  = 0.0f;
    // 直近のUpdateで何らかの入力を検知したか
    bool hasInput_                    = false;
    // 遷移先のActive状態
    AttractActiveState* activeState_  = nullptr;
};
