#pragma once

/// parent
#include "system/ISystem.h"

/// stl
#include <memory>

/// engine
#include "scene/Scene.h"

/// state
#include "component/camera/state/ICameraState.h"
#include <component/camera/CameraController.h>

/// event
#include "event/PlayerStateChangedEvent.h"

/// <summary>
/// カメラの追従処理を行うシステム
/// </summary>
class FollowCameraUpdateSystem
    : public OriGine::ISystem,
      public std::enable_shared_from_this<FollowCameraUpdateSystem> {
public:
    FollowCameraUpdateSystem() : ISystem(OriGine::SystemCategory::Movement) {}
    ~FollowCameraUpdateSystem() = default;
    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    /// <summary>
    /// プレイヤーの状態変化イベントに応じて、カメラの状態を切り替える
    /// </summary>
    /// <param name="cameraController"></param>
    void CameraStateTransition(OriGine::EntityHandle _handle, CameraController* _cameraController);

    /// <summary>
    /// プレイヤーの移動状態をカメラの移動状態に変換する
    /// プレイヤーの状態に応じて、カメラの挙動を切り替えるために使用される.
    /// </summary>
    /// <param name="_state"></param>
    /// <returns></returns>
    CameraMoveState ConvertToCameraState(PlayerMoveState _state);

    /// <summary>
    /// カメラの移動状態に応じたカメラステートオブジェクトを生成する
    /// </summary>
    /// <param name="type"></param>
    /// <param name="scene"></param>
    /// <param name="camera"></param>
    /// <param name="player"></param>
    /// <returns></returns>
    std::shared_ptr<ICameraState> CreateCameraState(
        CameraMoveState type,
        OriGine::Scene* scene,
        OriGine::EntityHandle camera,
        OriGine::EntityHandle player);

private:
    size_t playerStateChangedEventId_ = 0; ///< プレイヤーの状態変化イベントID
    PlayerStateChangedEvent latestPlayerStateChangedEvent_; ///< 最新のプレイヤー状態変化イベント
    bool hasStateChangeRequest_ = false;
};
