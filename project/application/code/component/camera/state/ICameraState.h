#pragma once

/// engine
#include "scene/Scene.h"
#include "entity/EntityHandle.h"

/// <summary>
/// カメラの動作状態を表す列挙体
/// </summary>
enum class CameraMoveState {
    IDLE,
    FOLLOW,    // Dash / FallDown 共用
    WALL_RUN,
};

/// <summary>
/// カメラステートのインターフェース
/// </summary>
class ICameraState {
public:
    ICameraState(OriGine::Scene* _scene, OriGine::EntityHandle _cameraEntityHandle, OriGine::EntityHandle _playerEntityHandle)
        : scene_(_scene)
        , cameraEntityHandle_(_cameraEntityHandle)
        , playerEntityHandle_(_playerEntityHandle) {}
    virtual ~ICameraState() = default;

    virtual void Initialize() = 0;
    virtual void Update()     = 0;
    virtual void Finalize()   = 0;

protected:
    OriGine::Scene* scene_                       = nullptr;
    OriGine::EntityHandle cameraEntityHandle_     = OriGine::EntityHandle();
    OriGine::EntityHandle playerEntityHandle_     = OriGine::EntityHandle();
};
