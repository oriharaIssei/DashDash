#include "FollowCameraUpdateSystem.h"

/// stl
#include <algorithm>
#include <cmath>

/// engine
#include "Engine.h"

#include "camera/CameraManager.h"
#include "messageBus/MessageBus.h"
// component
#include "component/Camera/CameraController.h"
#include "component/camera/state/ICameraState.h"
#include "component/transform/CameraTransform.h"
#include "component/transform/Transform.h"
// state
#include "component/camera/state/CameraFollowState.h"
#include "component/camera/state/CameraIdleState.h"
#include "component/camera/state/CameraWallRunState.h"

#include "component/player/state/PlayerState.h"

/// math
#include "math/Interpolation.h"
#include <numbers>

using namespace OriGine;

void FollowCameraUpdateSystem::Initialize() {
    playerStateChangedEventId_ = MessageBus::GetInstance()->Subscribe<PlayerStateChangedEvent>([this](const PlayerStateChangedEvent& event) {
        latestPlayerStateChangedEvent_ = event;
        hasStateChangeRequest_         = true;
    });
}

void FollowCameraUpdateSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<PlayerStateChangedEvent>(playerStateChangedEventId_);
}

void FollowCameraUpdateSystem::UpdateEntity(EntityHandle _handle) {
    auto* cameraController = GetComponent<CameraController>(_handle);
    auto* cameraTransform  = GetComponent<CameraTransform>(_handle);
    auto* transform        = GetComponent<Transform>(_handle);

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Camera");

    /// プレイヤーの状態変化イベントがあれば、カメラステートを切り替える
    CameraStateTransition(_handle, cameraController);

    if (cameraController->cameraState_) {
        cameraController->cameraState_->Update();
    } else {
        cameraController->cameraState_ = CreateCameraState(
            ConvertToCameraState(latestPlayerStateChangedEvent_.currentMoveState),
            GetScene(),
            _handle,
            latestPlayerStateChangedEvent_.playerEntityHandle);
    }

    auto* targetTranslate = GetComponent<Transform>(cameraController->followTargetEntity);
    if (targetTranslate) {
        // 自動注視処理
        if (cameraController->isAutoLookAtPlayer) {
            Vec3f toTarget     = Vec3f::Normalize(targetTranslate->GetWorldTranslate() - cameraTransform->translate);
            float targetAngleY = std::atan2(toTarget[X], toTarget[Z]);

            float currentY                          = cameraController->destinationAngleXY[Y];
            cameraController->destinationAngleXY[Y] = LerpAngleByDeltaTime(currentY, targetAngleY, deltaTime, cameraController->interTargetInterpolation[Y]);
        }

        Matrix4x4 cameraRotateMat = MakeMatrix4x4::RotateX(cameraController->destinationAngleXY[X]) * MakeMatrix4x4::RotateY(cameraController->destinationAngleXY[Y]);

        // ======== ターゲット追従補間 ======== //
        Vec3f followTargetPosition = Vec3f(targetTranslate->GetWorldTranslate());

        Vec3f worldDelta = followTargetPosition - cameraController->interTarget;
        // 純回転行列の逆行列 = 転置
        Matrix4x4 invRotateMat = Matrix4x4::Transpose(cameraRotateMat);
        Vec3f localDelta       = worldDelta * invRotateMat;
        Vec3f followDest       = cameraController->interTarget + (localDelta * cameraRotateMat);

        // interTarget・followDest をカメラローカルに変換してLerp し、ワールドに戻す
        Vec3f localInterTarget  = cameraController->interTarget * invRotateMat;
        Vec3f localFollowDest   = followDest * invRotateMat;
        Vec3f localInterpolated = Vec3f(
            LerpByDeltaTime(localInterTarget[X], localFollowDest[X], deltaTime, cameraController->interTargetInterpolation[X]),
            LerpByDeltaTime(localInterTarget[Y], localFollowDest[Y], deltaTime, cameraController->interTargetInterpolation[Y]),
            LerpByDeltaTime(localInterTarget[Z], localFollowDest[Z], deltaTime, cameraController->interTargetInterpolation[Z]));
        cameraController->interTarget = localInterpolated * cameraRotateMat;

        // ======== 注視XY補間: ローカル空間で補間してからワールドに戻す ======== //
        Vec3f localInterLookAtTarget  = cameraController->interLookAtTarget * invRotateMat;
        Vec3f localFollowLookAtTarget = followTargetPosition * invRotateMat;
        localInterLookAtTarget[X]     = LerpByDeltaTime(
            localInterLookAtTarget[X], localFollowLookAtTarget[X],
            deltaTime, cameraController->interLookAtTargetInterpolation[X]);
        localInterLookAtTarget[Y] = LerpByDeltaTime(
            localInterLookAtTarget[Y], localFollowLookAtTarget[Y],
            deltaTime, cameraController->interLookAtTargetInterpolation[Y]);
        localInterLookAtTarget[Z]           = localInterpolated[Z];
        cameraController->interLookAtTarget = localInterLookAtTarget * cameraRotateMat;

        // ======== 注視点 (targetOffset) ======== //
        Vec3f lookAtBase     = cameraController->interLookAtTarget;
        Vec3f targetPosition = lookAtBase + (cameraController->currentTargetOffset * cameraRotateMat);

        // ======== カメラ位置 (offset) ======== //
        Vec3f cameraPos            = cameraController->interTarget + (cameraController->currentOffset * cameraRotateMat);
        cameraTransform->translate = cameraPos;

        // ======== カメラ回転 ======== //
        Vec3f lookDir                = Vec3f::Normalize(targetPosition - cameraTransform->translate);
        Quaternion targetQuat        = Quaternion::LookAt(lookDir, axisY);
        cameraController->baseRotate = SlerpByDeltaTime(
            cameraController->baseRotate,
            targetQuat,
            deltaTime,
            cameraController->rotateSensitivity)
                                           .normalize();

        cameraTransform->rotate = cameraController->baseRotate * Quaternion::RotateAxisAngle(axisZ, cameraController->currentRotateZ);

        // transform に同期
        transform->rotate    = cameraTransform->rotate;
        transform->translate = cameraTransform->translate;
        transform->UpdateMatrix();
    }

    cameraTransform->UpdateMatrix();
    CameraManager::GetInstance()->SetTransform(GetScene(), *cameraTransform);
    CameraManager::GetInstance()->DataConvertToBuffer(GetScene());
}

void FollowCameraUpdateSystem::CameraStateTransition(OriGine::EntityHandle _handle, CameraController* _cameraController) {
    if (!hasStateChangeRequest_) {
        return;
    }
    hasStateChangeRequest_ = false;

    if (_cameraController->followTargetEntity != latestPlayerStateChangedEvent_.playerEntityHandle) {
        return;
    }

    auto nextType = ConvertToCameraState(
        latestPlayerStateChangedEvent_.currentMoveState);

    // ★ 同じなら何もしない
    if (nextType == _cameraController->currentCameraStateType_) {
        return;
    }

    _cameraController->currentCameraStateType_ = nextType;

    if (_cameraController->cameraState_) {
        _cameraController->cameraState_->Finalize();
    }

    _cameraController->cameraState_ = CreateCameraState(
        nextType,
        GetScene(),
        _handle,
        latestPlayerStateChangedEvent_.playerEntityHandle);

    _cameraController->cameraState_->Initialize();
}

CameraMoveState FollowCameraUpdateSystem::ConvertToCameraState(PlayerMoveState _state) {
    switch (_state) {
    case PlayerMoveState::IDLE:
        return CameraMoveState::IDLE;

    case PlayerMoveState::DASH:
    case PlayerMoveState::FALL_DOWN:
    case PlayerMoveState::JUMP:
    case PlayerMoveState::WALL_JUMP:
    case PlayerMoveState::RUN_ON_RAIL:
        return CameraMoveState::FOLLOW;
    case PlayerMoveState::WALL_RUN:
        return CameraMoveState::WALL_RUN;

    default:
        return CameraMoveState::IDLE;
    }
}

std::shared_ptr<ICameraState> FollowCameraUpdateSystem::CreateCameraState(
    CameraMoveState _type,
    Scene* _scene,
    EntityHandle _camera,
    EntityHandle _player) {
    switch (_type) {
    case CameraMoveState::IDLE:
        return std::make_unique<CameraIdleState>(_scene, _camera, _player);

    case CameraMoveState::FOLLOW:
        return std::make_unique<CameraFollowState>(_scene, _camera, _player);

    case CameraMoveState::WALL_RUN:
        return std::make_unique<CameraWallRunState>(_scene, _camera, _player);
    }

    return nullptr;
}
