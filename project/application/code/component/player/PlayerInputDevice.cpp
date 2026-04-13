#include "PlayerInputDevice.h"

/// ECS
// component
#include "PlayerInput.h"

OriGine::Vec2f GamepadInputDevice::GetMoveDirection() const {
    return pad_->GetLeftStick().normalize();
}

bool GamepadInputDevice::IsJumpTrigger() const {
    for (auto b : player_->GetJumpButton()) {
        if (pad_->IsTrigger(b)) {
            return true;
        }
    }
    return false;
}

bool GamepadInputDevice::IsJumpPress() const {
    for (auto b : player_->GetJumpButton()) {
        if (pad_->IsPress(b))
            return true;
    }
    return false;
}

bool GamepadInputDevice::IsActive() const {
    return pad_->IsActive();
}

OriGine::Vec2f KeyboardInputDevice::GetMoveDirection() const {
    float x = 0.0f;
    float y = 0.0f;

    for (auto k : player_->GetMoveRightKeys())
        if (key_->IsPress(k))
            x += 1.0f;
    for (auto k : player_->GetMoveLeftKeys())
        if (key_->IsPress(k))
            x -= 1.0f;
    for (auto k : player_->GetMoveFrontKeys())
        if (key_->IsPress(k))
            y += 1.0f;
    for (auto k : player_->GetMoveBackKeys())
        if (key_->IsPress(k))
            y -= 1.0f;

    return OriGine::Vec2f(x, y).normalize();
}

bool KeyboardInputDevice::IsJumpTrigger() const {
    for (auto k : player_->GetJumpKeys()) {
        if (key_->IsTrigger(k))
            return true;
    }
    return false;
}

bool KeyboardInputDevice::IsJumpPress() const {
    for (auto k : player_->GetJumpKeys()) {
        if (key_->IsPress(k))
            return true;
    }
    return false;
}

bool KeyboardInputDevice::IsActive() const {
    // キーボードは常にアクティブ扱い
    return true;
}
