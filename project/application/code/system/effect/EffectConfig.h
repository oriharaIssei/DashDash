#pragma once

#include <cstdint>

namespace EffectConfig {

// CameraShake
namespace CameraShake {
constexpr float kNoiseCoefficient = 1.37f;
constexpr float kNoiseOffset      = 0.5f;
} // namespace CameraShake

// Spline (General)
namespace Spline {
constexpr float kHalfMultiplier = 0.5f;
} // namespace Spline

// TireSpline
namespace TireSpline {
constexpr int32_t kMinPoints = 4;
} // namespace TireSpline

// TireTrail (Bank Angles)
namespace TireTrail {
constexpr float kMaxBankAngle   = 0.74f;
constexpr float kPointThreshold = 0.3f;
constexpr int32_t kMaxSplit     = 18;
} // namespace TireTrail

// BackFire (Scales)
namespace BackFire {
constexpr float kMinScaleY  = 0.26f;
constexpr float kMaxScaleY  = 1.59f;
constexpr float kMinScaleXZ = 0.66f;
constexpr float kMaxScaleXZ = 1.26f;
} // namespace BackFire

} // namespace EffectConfig
