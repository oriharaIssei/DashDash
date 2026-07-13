#pragma once
#include <stdint.h>

namespace AppConfig {
namespace Spline {

constexpr int32_t kMinLinePoints       = 2; // 直線描画に必要な最小制御点数
constexpr int32_t kMinCatmullRomPoints = 4; // Catmull-Romスプラインに必要な最小制御点数

} // namespace Spline
} // namespace AppConfig
