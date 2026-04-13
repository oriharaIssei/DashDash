#pragma once

#include "CameraConfig.h"

#include "math/Vector3.h"

/// <summary>
/// カメラステートごとのパラメータ
/// </summary>
struct CameraStateParams {
    OriGine::Vec3f targetOffset              = {0.0f, 0.0f, 0.0f};
    OriGine::Vec3f offset                    = {0.0f, 0.0f, 0.0f};
    OriGine::Vec3f interTargetInterpolation  = AppConfig::Camera::kDefaultInterTargetInterpolation;
    OriGine::Vec3f interLookAtTargetInterpolation = AppConfig::Camera::kDefaultInterTargetInterpolation;
};
