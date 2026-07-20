#pragma once

#include "math/MathEnv.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

namespace AppConfig {
// カメラ関連のデフォルト値・定数をまとめた名前空間
namespace Camera {

constexpr float kDefaultTilt                              = 0.33f; // カメラの傾き係数のデフォルト値
constexpr OriGine::Vec3f kDefaultInterTargetInterpolation = {5.0f, 5.0f, 5.0f}; // 追従目標への補間速度のデフォルト値
constexpr float kDefaultRotateSensitivity                 = 10.0f; // カメラ回転感度のデフォルト値

// Offsets
constexpr OriGine::Vec3f kFirstOffset       = {0.0f, 0.0f, -10.0f}; // Idle状態でのカメラ位置オフセット
constexpr OriGine::Vec3f kFirstTargetOffset = {0.0f, 0.0f, 1.0f}; // Idle状態での注視点オフセット

constexpr OriGine::Vec3f kTargetOffsetOnDash = {0.0f, -0.5f, 1.0f}; // Dash/WallRun状態での注視点オフセット

constexpr OriGine::Vec3f kOffsetOnDash = {0.0f, 0.5f, -12.0f}; // Dash/WallRun状態でのカメラ位置オフセット

constexpr float kFixForForwardSpeed = 10.0f; // 前進速度補正の基準値

constexpr OriGine::Vec2f kDefaultDestinationAngle = {-0.1f, 0.0f}; // カメラ目標角度(XY)のデフォルト値

// FOV
constexpr float kDefaultFovYInterpolate = 0.1f; // FovYの補間係数のデフォルト値
constexpr float kDefaultFovMin          = 45.0f * OriGine::kDeg2Rad; // FovYの最小値（ラジアン）
constexpr float kDefaultFovMax          = 55.0f * OriGine::kDeg2Rad; // FovYの最大値（ラジアン）
constexpr float kDefaultFovMinSpeed     = 0.0f; // FovY補間の基準となる最小速度
constexpr float kDefaultFovMaxSpeed     = 30.0f; // FovY補間の基準となる最大速度

} // namespace Camera
} // namespace AppConfig
