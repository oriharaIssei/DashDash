#pragma once

#include "system/ISystem.h"

/// stl
#include <memory>

/// state
#include "state/AttractModeStates.h"

/// <summary>
/// アトラクションモードのシステム
/// (未入力時間が一定以上になったら、Title画面などのSubSceneを起動する)
/// </summary>
class AttractModeSystem
    : public OriGine::ISystem {
public:
    AttractModeSystem();
    ~AttractModeSystem() override = default;

    void Initialize() override;
    void Finalize() override;

private:
    void Update() override;
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

private:
    static constexpr float kAttractModeThreshold_     = 30.0f;
    static constexpr float kSceneInitializationDelay_ = 1.0f;

    AttractActiveState activeState_;
    AttractIdleState idleState_;
    AttractInitializingState initializingState_;

    IAttractModeState* currentState_  = nullptr;
    IAttractModeState* previousState_ = nullptr;
};
