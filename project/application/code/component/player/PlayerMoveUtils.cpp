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

    Vec2f dirVel = {Vec3f::Dot(_velocity, rightDirection), Vec3f::Dot(_velocity, _forwardDirection)};

    Vec2f targetDirSpeed = _playerStatus->GetCurrentMaxDirectionalSpeed();
    targetDirSpeed[X] *= _sideInput;

    float speedT     = Vec2f(_velocity[X], _velocity[Z]).length() / _playerStatus->CalculateSpeedByGearLevel(kMaxPlayerGearLevel);
    Vec2f smoothTime = Lerp(_playerStatus->GetMinSmoothTime(), _playerStatus->GetMaxSmoothTime(), speedT);
    Vec2f limitSpeed = Lerp(_playerStatus->GetMinLimitDirectionalAccel(), _playerStatus->GetMaxLimitDirectionalAccel(), speedT);

    Vec2f currentDirSpeed = _playerStatus->GetCurrentDirectionalSpeed();
    dirVel[X]             = OriGine::SmoothDamp(dirVel[X], targetDirSpeed[X], currentDirSpeed[X], smoothTime[X], _deltaTime, limitSpeed[X]);
    dirVel[Y]             = OriGine::SmoothDamp(dirVel[Y], targetDirSpeed[Y], currentDirSpeed[Y], smoothTime[Y], _deltaTime, limitSpeed[Y]);
    _playerStatus->SetCurrentDirectionalSpeed(currentDirSpeed);

    Vec3f result = _forwardDirection * dirVel[Y] + rightDirection * dirVel[X];

    float currentMaxSpeed = _playerStatus->GetCurrentMaxSpeed();
    if (result.lengthSq() >= currentMaxSpeed * currentMaxSpeed) {
        result = result.normalize() * currentMaxSpeed;
    }

    return result + Vec3f(0.f, _velocity[Y], 0.f);
}

}
