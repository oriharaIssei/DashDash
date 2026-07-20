#include "PlayerMoveUtils.h"

/// ECS
#include "component/player/state/PlayerState.h"

/// math
#include "SpringDamper.h"

using namespace OriGine;

namespace PlayerMoveUtils {

bool IsHitGround(const Vec3f& _collNormal) {
    return _collNormal[Y] > kGroundCheckThreshold;
}

WallContactResult EvaluateWallContact(float _parallelFactor, const PlayerStatus* _status) {
    // 壁に沿って移動している場合は壁衝突とみなす
    if (_parallelFactor > _status->GetWallCheckThreshold()) {
        if (_status->CanWallRun()) {
            return WallContactResult::WallRun;
        }
    }
    return WallContactResult::WallHit;
}

OriGine::Vec3f ComputeWallRunDirection(const OriGine::Vec3f& _velocity, const OriGine::Vec3f& _wallNormal) {
    return _velocity - _wallNormal * OriGine::Vec3f::Dot(_velocity, _wallNormal);
}

/// <summary>
/// ウィリー（壁登り）時の登坂方向を計算する
/// </summary>
/// <param name="_wallNormal">壁の法線</param>
/// <param name="_up">上方向</param>
/// <returns>登坂方向</returns>
OriGine::Vec3f ComputeWheelieDirection(const OriGine::Vec3f& _wallNormal, const OriGine::Vec3f& _up) {
    float dot            = Vec3f::Dot(_wallNormal, _up);
    Vec3f climbDirection = axisY - (_wallNormal * dot);
    climbDirection       = climbDirection.normalize();
    return climbDirection;
}

bool IsWallRight(const OriGine::Vec3f& _direction, const OriGine::Vec3f& _wallNormal, const OriGine::Vec3f& _up) {
    return Vec3f::Dot(Vec3f::Cross(_up, _wallNormal), _direction) > 0.0f;
}

OriGine::Vec3f UpdatePlanarVelocity(PlayerStatus* _playerStatus, const Vec3f& _velocity, float _sideInput, const Vec3f& _forwardDirection, float _deltaTime) {
    Vec3f rightDirection = axisY.cross(_forwardDirection).normalize();

    // 現在速度を正面/横方向成分に分解
    Vec2f dirVel = {Vec3f::Dot(_velocity, rightDirection), Vec3f::Dot(_velocity, _forwardDirection)};

    // 横入力に応じた目標横速度を算出
    Vec2f targetDirSpeed = _playerStatus->GetCurrentMaxDirectionalSpeed();
    targetDirSpeed[X] *= _sideInput;

    // 現在の最大ギア速度に対する速度割合から、速度に応じた減衰特性を補間する
    float speedT     = Vec2f(_velocity[X], _velocity[Z]).length() / _playerStatus->CalculateSpeedByGearLevel(kMaxPlayerGearLevel);
    Vec2f smoothTime = Lerp(_playerStatus->GetMinSmoothTime(), _playerStatus->GetMaxSmoothTime(), speedT);
    Vec2f limitSpeed = Lerp(_playerStatus->GetMinLimitDirectionalAccel(), _playerStatus->GetMaxLimitDirectionalAccel(), speedT);

    // 横/正面成分それぞれをスムーズダンプで目標速度へ近づける
    Vec2f currentDirSpeed = _playerStatus->GetCurrentDirectionalSpeed();
    dirVel[X]             = OriGine::SmoothDamp(dirVel[X], targetDirSpeed[X], currentDirSpeed[X], smoothTime[X], _deltaTime, limitSpeed[X]);
    dirVel[Y]             = OriGine::SmoothDamp(dirVel[Y], targetDirSpeed[Y], currentDirSpeed[Y], smoothTime[Y], _deltaTime, limitSpeed[Y]);
    _playerStatus->SetCurrentDirectionalSpeed(currentDirSpeed);

    Vec3f result = _forwardDirection * dirVel[Y] + rightDirection * dirVel[X];

    // 現在の最大速度を超えないようクランプ
    float currentMaxSpeed = _playerStatus->GetCurrentMaxSpeed();
    if (result.lengthSq() >= currentMaxSpeed * currentMaxSpeed) {
        result = result.normalize() * currentMaxSpeed;
    }

    return result + Vec3f(0.f, _velocity[Y], 0.f);
}

}
