#include "AttractModeStates.h"

/// engine
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"
#include "input/MouseInput.h"

/// math
#include "MathEnv.h"

using namespace OriGine;

//=============================================================================
// AttractInitializingState
//=============================================================================

void AttractInitializingState::Enter() {
    timer_ = 0.0f;
}

void AttractInitializingState::Update(float _deltaTime) {
    timer_ += _deltaTime;
}

IAttractModeState* AttractInitializingState::Transition() {
    if (timer_ >= delay_) {
        return activeState_;
    }
    return nullptr;
}

//=============================================================================
// AttractActiveState
//=============================================================================

void AttractActiveState::Update(float /*_deltaTime*/) {
    GamepadInput* gamepadInput   = scene_->GetGamepadInput();
    KeyboardInput* keyboardInput = scene_->GetKeyboardInput();
    MouseInput* mouseInput       = scene_->GetMouseInput();

    hasInput_ = (gamepadInput && gamepadInput->IsActive() && gamepadInput->IsPressAny())
                || (keyboardInput && keyboardInput->IsPressAnyKey())
                || (mouseInput && mouseInput->IsPressAnyButton() && (mouseInput->GetVelocity().lengthSq() > kEpsilon));
}

IAttractModeState* AttractActiveState::Transition() {
    if (hasInput_) {
        hasInput_ = false;
        return idleState_;
    }
    return nullptr;
}

//=============================================================================
// AttractIdleState
//=============================================================================

void AttractIdleState::Enter() {
    idleTimer_ = 0.0f;
    hasInput_  = false;
}

void AttractIdleState::Update(float _deltaTime) {
    GamepadInput* gamepadInput   = scene_->GetGamepadInput();
    KeyboardInput* keyboardInput = scene_->GetKeyboardInput();
    MouseInput* mouseInput       = scene_->GetMouseInput();

    hasInput_ = (gamepadInput && gamepadInput->IsActive() && gamepadInput->IsPressAny())
                || (keyboardInput && keyboardInput->IsPressAnyKey())
                || (mouseInput && mouseInput->IsPressAnyButton() && (mouseInput->GetVelocity().lengthSq() > kEpsilon));

    if (hasInput_) {
        idleTimer_ = 0.0f;
    } else {
        idleTimer_ += _deltaTime;
    }
}

IAttractModeState* AttractIdleState::Transition() {
    if (idleTimer_ >= threshold_) {
        return activeState_;
    }
    return nullptr;
}
