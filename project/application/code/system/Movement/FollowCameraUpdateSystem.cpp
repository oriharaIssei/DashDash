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

/// <summary>
/// 追従カメラを1フレーム分更新する。
/// カメラ位置と注視点をそれぞれ別の速度で遅れて追従させることで、
/// 硬直した「棒付きカメラ」ではなく、慣性のある滑らかな見え方を作っている。
/// </summary>
/// <remarks>
/// 処理は次の順序で行う。順序には依存関係があるため入れ替えないこと。
/// ①プレイヤーの状態変化に応じてカメラステートを差し替え、ステートごとのオフセット等を更新する
/// ②追従位置(interTarget)をカメラローカル空間で補間する
/// ③注視点(interLookAtTarget)を同じくローカル空間で補間する。②のZ成分を再利用するため②の後
/// ④②③のオフセットからカメラ位置・注視点を確定し、そこを向く回転をSlerpで求める
/// 補間をワールドではなくカメラローカルで行うのは、「奥行き方向」と「左右方向」で
/// 追従の効き具合を別々に調整できるようにするため。
/// </remarks>
void FollowCameraUpdateSystem::UpdateEntity(const EntityHandle& _handle) {
    auto* cameraController = GetComponent<CameraController>(_handle);
    auto* cameraTransform  = GetComponent<CameraTransform>(_handle);
    auto* transform        = GetComponent<Transform>(_handle);

    const float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Camera");

    /// プレイヤーの状態変化イベントがあれば、カメラステートを切り替える
    CameraStateTransition(_handle, cameraController);

    if (cameraController->GetCameraState()) {
        cameraController->GetCameraState()->Update();
    } else {
        cameraController->SetCameraState(CreateCameraState(
            ConvertToCameraState(latestPlayerStateChangedEvent_.currentMoveState),
            GetScene(),
            _handle,
            latestPlayerStateChangedEvent_.playerEntityHandle));
    }

    auto* targetTranslate = GetComponent<Transform>(cameraController->GetFollowTargetEntity());
    if (targetTranslate) {
        // 自動注視処理
        if (cameraController->GetIsAutoLookAtPlayer()) {
            Vec3f toTarget     = Vec3f::Normalize(targetTranslate->GetWorldTranslate() - cameraTransform->translate);
            // 左手座標系ではヨー角は+Z(前方)を0として測るため、atan2の引数は(Z,X)ではなく(X,Z)の順。
            // 逆にするとカメラが90度ずれた方向を向く
            float targetAngleY = std::atan2(toTarget[X], toTarget[Z]);

            Vec2f angleXY = cameraController->GetDestinationAngleXY();
            angleXY[Y]    = LerpAngleByDeltaTime(angleXY[Y], targetAngleY, deltaTime, cameraController->GetInterTargetInterpolation()[Y]);
            cameraController->SetDestinationAngleXY(angleXY);
        }

        Vec2f destAngle         = cameraController->GetDestinationAngleXY();
        Matrix4x4 cameraRotateMat = MakeMatrix4x4::RotateX(destAngle[X]) * MakeMatrix4x4::RotateY(destAngle[Y]);

        // ======== ターゲット追従補間 ======== //
        Vec3f followTargetPosition = Vec3f(targetTranslate->GetWorldTranslate());

        Vec3f interTarget = cameraController->GetInterTarget();
        Vec3f worldDelta  = followTargetPosition - interTarget;
        // 純回転行列の逆行列 = 転置
        // (逆行列を解く必要がないので、以降のワールド→カメラローカル変換は全てこれを使い回す)
        Matrix4x4 invRotateMat = Matrix4x4::Transpose(cameraRotateMat);
        Vec3f localDelta       = worldDelta * invRotateMat;
        // NOTE: invRotateMatで回してcameraRotateMatで戻しているため、この変換は数学的には往復して
        // 元に戻り、followDest は followTargetPosition と一致する。
        // 軸ごとに補間の効きを変えるための布石として書かれているが現状は成分の加工をしていない
        Vec3f followDest       = interTarget + (localDelta * cameraRotateMat);

        // 追従の遅れをワールド軸ではなくカメラ軸で効かせたいので、両端点をカメラローカルに変換してから
        // 軸ごとに別々の補間率でLerpし、最後にワールドへ戻す。
        // これにより「カメラの奥行き方向はゆっくり、左右方向は素早く」といった調整が可能になる
        Vec3f localInterTarget  = interTarget * invRotateMat;
        Vec3f localFollowDest   = followDest * invRotateMat;
        Vec3f interpVec         = cameraController->GetInterTargetInterpolation();
        Vec3f localInterpolated = Vec3f(
            LerpByDeltaTime(localInterTarget[X], localFollowDest[X], deltaTime, interpVec[X]),
            LerpByDeltaTime(localInterTarget[Y], localFollowDest[Y], deltaTime, interpVec[Y]),
            LerpByDeltaTime(localInterTarget[Z], localFollowDest[Z], deltaTime, interpVec[Z]));
        cameraController->SetInterTarget(localInterpolated * cameraRotateMat);

        // ======== 注視XY補間: ローカル空間で補間してからワールドに戻す ======== //
        Vec3f lookAtInterpVec        = cameraController->GetInterLookAtTargetInterpolation();
        Vec3f localInterLookAtTarget = cameraController->GetInterLookAtTarget() * invRotateMat;
        Vec3f localFollowLookAtTarget = followTargetPosition * invRotateMat;
        localInterLookAtTarget[X]     = LerpByDeltaTime(
            localInterLookAtTarget[X], localFollowLookAtTarget[X],
            deltaTime, lookAtInterpVec[X]);
        localInterLookAtTarget[Y] = LerpByDeltaTime(
            localInterLookAtTarget[Y], localFollowLookAtTarget[Y],
            deltaTime, lookAtInterpVec[Y]);
        // 注視点の奥行き(Z)だけは独自に補間せず、カメラ位置側の補間結果をそのまま流用する。
        // 別々に補間すると注視点とカメラの前後間隔が伸縮し、画面内のプレイヤーの大きさが
        // 不規則に変化してしまうため、奥行きは常に同じ量だけ動かして間隔を一定に保つ
        localInterLookAtTarget[Z] = localInterpolated[Z];
        cameraController->SetInterLookAtTarget(localInterLookAtTarget * cameraRotateMat);

        // ======== 注視点 (targetOffset) ======== //
        Vec3f lookAtBase     = cameraController->GetInterLookAtTarget();
        Vec3f targetPosition = lookAtBase + (cameraController->GetCurrentTargetOffset() * cameraRotateMat);

        // ======== カメラ位置 (offset) ======== //
        Vec3f cameraPos            = cameraController->GetInterTarget() + (cameraController->GetCurrentOffset() * cameraRotateMat);
        cameraTransform->translate = cameraPos;

        // ======== カメラ回転 ======== //
        // 角度を直接補間するとジンバルロックや-π/+πの巻き戻りが起きるため、
        // 目標姿勢をクォータニオンで求めてSlerpする。
        // Slerpの結果は累積誤差で長さが1からずれていくので、毎フレームnormalizeして正規化を保つ
        Vec3f lookDir                = Vec3f::Normalize(targetPosition - cameraTransform->translate);
        Quaternion targetQuat        = Quaternion::LookAt(lookDir, axisY);
        Quaternion newBaseRotate     = SlerpByDeltaTime(
            cameraController->GetBaseRotate(),
            targetQuat,
            deltaTime,
            cameraController->GetRotateSensitivity())
                                           .normalize();
        cameraController->SetBaseRotate(newBaseRotate);

        // Z軸ロール(カーブ時の傾き演出)はbaseRotateの「後ろ」に掛ける。
        // こうするとロールがカメラのローカルZ軸まわりに適用され、視線方向を保ったまま画面が傾く。
        // 順序を逆にするとワールドZ軸まわりの回転になり、注視点からずれてしまう
        cameraTransform->rotate = cameraController->GetBaseRotate() * Quaternion::RotateAxisAngle(axisZ, cameraController->GetCurrentRotateZ());

        // transform に同期
        transform->rotate    = cameraTransform->rotate;
        transform->translate = cameraTransform->translate;
        transform->UpdateMatrix();
    }

    cameraTransform->UpdateMatrix();
    CameraManager::GetInstance()->SetTransform(GetScene(), *cameraTransform);
    CameraManager::GetInstance()->DataConvertToBuffer(GetScene());
}

void FollowCameraUpdateSystem::CameraStateTransition(const OriGine::EntityHandle& _handle, CameraController* _cameraController) {
    if (!hasStateChangeRequest_) {
        return;
    }
    hasStateChangeRequest_ = false;

    // イベントはMessageBus経由で全カメラに配信されるため、自分が追従していないプレイヤーの
    // 状態変化で切り替わってしまわないよう、対象が一致するものだけを処理する
    if (_cameraController->GetFollowTargetEntity() != latestPlayerStateChangedEvent_.playerEntityHandle) {
        return;
    }

    auto nextType = ConvertToCameraState(
        latestPlayerStateChangedEvent_.currentMoveState);

    // ★ 同じなら何もしない
    // 複数のPlayerMoveStateが同一のCameraMoveStateに写像される(DASHとJUMPはどちらもFOLLOW)ため、
    // プレイヤー側が遷移してもカメラ側は据え置きになるケースがある。
    // ここで弾かないとステートを作り直してしまい、補間中のオフセットが初期値に戻ってカメラが飛ぶ
    if (nextType == _cameraController->GetCurrentCameraStateType()) {
        return;
    }

    _cameraController->SetCurrentCameraStateType(nextType);

    if (_cameraController->GetCameraState()) {
        _cameraController->GetCameraState()->Finalize();
    }

    _cameraController->SetCameraState(CreateCameraState(
        nextType,
        GetScene(),
        _handle,
        latestPlayerStateChangedEvent_.playerEntityHandle));

    _cameraController->GetCameraState()->Initialize();
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
    const EntityHandle& _camera,
    const EntityHandle& _player) {
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
