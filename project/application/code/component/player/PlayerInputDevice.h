#pragma once

/// engine
// input
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"

/// ECS
// component
class PlayerInput;

/// math
#include "math/Vector2.h"

/// <summary>
/// プレイヤーの入力デバイス インターフェース
/// </summary>
class IPlayerInputDevice {
public:
    virtual ~IPlayerInputDevice() = default;

    // 移動方向（-1〜1の正規化入力）
    virtual OriGine::Vec2f GetMoveDirection() const = 0;

    // ジャンプ
    virtual bool IsJumpTrigger() const = 0;
    virtual bool IsJumpPress() const   = 0;

    // デバイスが有効かどうか（パッド優先にする時に使う）
    virtual bool IsActive() const = 0;

    virtual bool IsAnyInput() const {
        return GetMoveDirection().lengthSq() > 0.0f || IsJumpPress();
    }
};

/// <summary>
/// ゲームパッド入力デバイス
/// </summary>
class GamepadInputDevice
    : public IPlayerInputDevice {
public:
    GamepadInputDevice(OriGine::GamepadInput* _pad, PlayerInput* _player)
        : pad_(_pad), player_(_player) {}

    OriGine::Vec2f GetMoveDirection() const override;

    bool IsJumpTrigger() const override;

    bool IsJumpPress() const override;

    bool IsActive() const override;

private:
    OriGine::GamepadInput* pad_;
    PlayerInput* player_;
};

/// <summary>
/// キーボード入力デバイス
/// </summary>
class KeyboardInputDevice
    : public IPlayerInputDevice {
public:
    KeyboardInputDevice(OriGine::KeyboardInput* _key, PlayerInput* _player)
        : key_(_key), player_(_player) {}

    OriGine::Vec2f GetMoveDirection() const override;

    bool IsJumpTrigger() const override;

    bool IsJumpPress() const override ;

    bool IsActive() const override;

private:
    OriGine::KeyboardInput* key_;
    PlayerInput* player_;
};
