#pragma once

#include "CameraConfig.h"

#include "math/Vector3.h"

/// <summary>
/// カメラステートごとのパラメータ
/// </summary>
struct CameraStateParams {
    OriGine::Vec3f targetOffset              = {0.0f, 0.0f, 0.0f}; // 注視点のオフセット
    OriGine::Vec3f offset                    = {0.0f, 0.0f, 0.0f}; // カメラ位置のオフセット
    OriGine::Vec3f interTargetInterpolation  = AppConfig::Camera::kDefaultInterTargetInterpolation; // カメラ位置補間の速度
    OriGine::Vec3f interLookAtTargetInterpolation = AppConfig::Camera::kDefaultInterTargetInterpolation; // 注視点補間の速度
};
