#pragma once

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "PlayerStatus.h"

/// math
#include "math/Vector3.h"

namespace PlayerMoveUtils {
static const float kGroundCheckThreshold = 0.7f; // 地面と判断するための閾値
static const float kWallCheckThreshold   = 0.42f; // 壁と判断するための閾値

/// <summary>
/// 地面に衝突したか
/// </summary>
/// <param name="_collNormal">衝突法線</param>
/// <returns>地面に衝突したか</returns>
bool IsHitGround(const OriGine::Vec3f& _collNormal);

/// <summary>
/// Playerが壁に接触した際の挙動を評価する
/// </summary>
enum class WallContactResult {
    WallRun,
    WallHit
};

/// <summary>
/// Player が壁に接触した際の挙動を評価する
/// </summary>
/// <param name=""></param>
/// <returns></returns>
WallContactResult EvaluateWallContact(
    float _parallelFactor,
    const PlayerStatus* _status);

/// <summary>
/// 壁走りの移動方向を計算する
/// </summary>
/// <param name="_velocity"></param>
/// <param name="_wallNormal"></param>
/// <returns> 壁走りの移動方向 </returns>
OriGine::Vec3f ComputeWallRunDirection(const OriGine::Vec3f& _velocity, const OriGine::Vec3f& _wallNormal);

/// <summary>
/// 壁が右側にあるかを判定する
/// </summary>
/// <param name="_direction"></param>
/// <param name="_wallNormal"></param>
/// <param name="_up"></param>
/// <returns>壁が右側</returns>
bool IsWallRight(const OriGine::Vec3f& _direction, const OriGine::Vec3f& _wallNormal, const OriGine::Vec3f& _up = OriGine::axisY);

/// <summary>
/// XZ 平面の速度を更新する
/// </summary>
/// <param name="_playerStatus"></param>
/// <param name="_velocity">現在の速度</param>
/// <param name="_sideInput">横入力</param>
/// <param name="_forwardDirection">現在の正面方向</param>
/// <param name="_deltaTime"></param>
/// <returns></returns>
OriGine::Vec3f UpdatePlanarVelocity(
    PlayerStatus* _playerStatus,
    const OriGine::Vec3f& _velocity,
    float _sideInput,
    const OriGine::Vec3f& _forwardDirection,
    float _deltaTime);

} // namespace PlayerMoveUtils
