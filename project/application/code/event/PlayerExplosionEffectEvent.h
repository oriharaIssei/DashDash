#pragma once

#include "math/Vector3.h"

/// <summary>
/// Playerが爆発するEvent
/// </summary>
struct PlayerExplosionEffectEvent {
    PlayerExplosionEffectEvent()  = default;
    ~PlayerExplosionEffectEvent() = default;

    OriGine::Vec3f position; ///< 爆発する位置
};
