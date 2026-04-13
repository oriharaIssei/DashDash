#pragma once
#include "math/Vector4.h"

namespace AppConfig {
namespace UI {

// Button Colors
constexpr OriGine::Vec4f kButtonNormalColor  = {0.32f, 0.32f, 0.32f, 1.0f};
constexpr OriGine::Vec4f kButtonHoverColor   = {0.8f, 0.8f, 0.8f, 1.0f};
constexpr OriGine::Vec4f kButtonPressColor   = {0.6f, 0.6f, 0.6f, 1.0f};
constexpr OriGine::Vec4f kButtonReleaseColor = {1.0f, 1.0f, 1.0f, 1.0f};

// Scene Transition
constexpr float kMaxSceneTransitionTime = 0.8f;

} // namespace UI
} // namespace AppConfig
