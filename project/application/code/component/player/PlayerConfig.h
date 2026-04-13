#pragma once
#include <stdint.h>

namespace AppConfig {
namespace Player {

constexpr float kDefaultHeight             = 2.0f;
constexpr float kDefaultBaseGearupCoolTime = 1.0f;
constexpr float kDefaultWallRunRampUpTime  = 0.2f;
constexpr float kDefaultScale              = 1.0f;
constexpr float kWallRunOffsetRate         = 0.1f;
constexpr float kWallRunMeshOffsetRate     = 0.26f;

// Dash
constexpr int32_t kDashCameraOffsetThresholdGearLevel = 2;
constexpr float kDashCameraOffsetLerpTime             = 2.3f;
constexpr float kDashFallDownThresholdTime            = 0.3f;

// Status Defaults
constexpr float kDefaultGravityApplyDelayOnWallRun = 0.2f;
constexpr float kDefaultWallRunDetachSpeed         = 5.0f;
constexpr float kDefaultInvincibilityTime          = 1.0f;
constexpr float kDefaultMass                       = 1.0f;
constexpr float kDefaultMassOnWallRun              = 0.3f;
constexpr float kDefaultWallRunInterval            = 0.2f;

// Wheelie
constexpr float kWheelieUpwardSpeedCorrectionValue = 0.4f;
constexpr float kWheelieWallOffset                 = 0.1f;
constexpr float kWheelieSeparationGraceTime        = 0.1f;
constexpr float kWheelieFallThresholdVelocity      = 0.0f;

// Penalty
constexpr int32_t kPenaltyThresholdGearLevel = 4;
constexpr int32_t kPenaltyGearLevelDivisor   = 2;

// Fall Detection
constexpr float kFallThresholdY = -50.0f;

// Effects
constexpr int32_t kGearUpEffectShockWaveThresholdGearLevel = 2;
constexpr int32_t kGearUpEffectTrailAnimationIndex         = 1;

// Thresholds
constexpr float kGroundCheckThreshold        = 0.7f;
constexpr float kWallCheckThreshold          = 0.42f;
constexpr int32_t kWallRunThresholdGearLevel = 3;

} // namespace Player
} // namespace AppConfig
