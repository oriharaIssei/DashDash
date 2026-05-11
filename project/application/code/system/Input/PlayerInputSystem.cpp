#include "PlayerInputSystem.h"

/// engine
#define ENGINE_ECS
#define DELTA_TIME
#include "EngineInclude.h"
#include "scene/Scene.h"
// input
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"

/// component
#include "component/player/PlayerInput.h"
#include "component/player/PlayerInputDevice.h"
#include "component/player/state/IPlayerMoveState.h"
#include "component/player/state/PlayerState.h"

using namespace OriGine;

void PlayerInputSystem::Initialize() {}
void PlayerInputSystem::Finalize() {}

void PlayerInputSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto keyInput = GetScene()->GetKeyboardInput();
    auto padInput = GetScene()->GetGamepadInput();

    auto playerInput = GetComponent<PlayerInput>(_handle);
    auto state       = GetComponent<PlayerState>(_handle);

    if (!playerInput || !state) {
        return;
    }

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");

    InputUpdate(
        deltaTime,
        keyInput,
        padInput,
        playerInput,
        state);
}

void PlayerInputSystem::InputUpdate(float _deltaTime, OriGine::KeyboardInput* _keyInput, OriGine::GamepadInput* _padInput, PlayerInput* _playerInput, PlayerState* _playerState) {

    GamepadInputDevice padDevice(_padInput, _playerInput);
    KeyboardInputDevice keyDevice(_keyInput, _playerInput);

    bool isUsingGamepad        = padDevice.IsActive();
    IPlayerInputDevice* device = SelectActiveDevice(isUsingGamepad, &padDevice, &keyDevice);

    // --- 移動 ---
    _playerInput->SetInputDirection(device->GetMoveDirection());

    // --- ジャンプ ---
    HandleJump(_deltaTime, _playerInput, _playerState, device);
}

IPlayerInputDevice* PlayerInputSystem::SelectActiveDevice(bool _preUsingGamepad, IPlayerInputDevice* _padDevice, IPlayerInputDevice* _keyDevice) {
    if (_padDevice->IsActive()) {
        if (_preUsingGamepad) {
            return _keyDevice->IsAnyInput() ? _keyDevice : _padDevice;
        } else {
            return _padDevice->IsAnyInput() ? _padDevice : _keyDevice;
        }
    }
    return _keyDevice;
}

void PlayerInputSystem::HandleJump(
    float _deltaTime,
    PlayerInput* input,
    PlayerState* state,
    IPlayerInputDevice* device) {

    input->SetWallJumpInput(false);
    input->SetRailJumpInput(false);

    auto moveState = state->GetPlayerMoveState();
    if (!moveState) {
        return;
    }

    if (input->IsJumpInput()) {
        if (!moveState->CanHoldJump()) {
            input->SetJumpInput(false);
            input->SetJumpInputTime(0.0f);
            return;
        }

        if (device->IsJumpPress()) {
            input->SetJumpInput(true);
            input->SetJumpInputTime(
                input->GetJumpInputTime() + _deltaTime);

            if (input->GetJumpInputTime() >= input->GetMaxJumpTime()) {
                input->SetJumpInput(false);
                input->SetJumpInputTime(0.0f);
            }
        } else {
            input->SetJumpInput(false);
            input->SetJumpInputTime(0.0f);
        }

    } else {
        if (device->IsJumpTrigger()) {
            input->SetJumpInput(true);

            JumpInputResponse response = moveState->OnJumpTriggered();
            input->SetWallJumpInput(response.wallJump);
            input->SetRailJumpInput(response.railJump);
        }
    }
}
