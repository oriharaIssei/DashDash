#pragma once
#include "system/ISystem.h"

namespace OriGine {
/// engine
// input
class KeyboardInput;
class GamepadInput;
}

/// ECS
// component
class IPlayerInputDevice;
class PlayerInput;
class PlayerState;

/// <summary>
/// PlayerのInputを処理するシステム
/// </summary>
class PlayerInputSystem
    : public OriGine::ISystem {
public:
    PlayerInputSystem() : ISystem(OriGine::SystemCategory::Input) {}
    ~PlayerInputSystem() {}

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    // void Update() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// エンティティの更新処理
    /// </summary>
    void UpdateEntity(OriGine::EntityHandle _handle) override;
    /// <summary>
    /// 入力処理の実行
    /// </summary>
    void InputUpdate(float _deltaTime, OriGine::KeyboardInput* _keyInput, OriGine::GamepadInput* _padInput, PlayerInput* _playerInput, PlayerState* _playerState);

private:
    /// <summary>
    /// 使用する入力デバイスを選択する
    /// </summary>
    IPlayerInputDevice* SelectActiveDevice(bool _preUsingGamepad, IPlayerInputDevice* _padDevice, IPlayerInputDevice* _keyDevice);
    /// <summary>
    /// ジャンプ入力の処理
    /// </summary>
    void HandleJump(float _deltaTime, PlayerInput* _playerInput, PlayerState* _state, IPlayerInputDevice* _device);
};
