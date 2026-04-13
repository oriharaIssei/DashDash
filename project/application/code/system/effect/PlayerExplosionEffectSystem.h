#pragma once

#include "system/ISystem.h"

/// stl
#include <memory>
#include <vector>

/// event
#include "event/PlayerExplosionEffectEvent.h"

/// <summary>
/// PlayerExplosionEffectEvent を受け取って爆発エフェクトを処理するシステム
/// </summary>
class PlayerExplosionEffectSystem
    : public OriGine::ISystem,
      public std::enable_shared_from_this<PlayerExplosionEffectSystem> {
public:
    PlayerExplosionEffectSystem();
    ~PlayerExplosionEffectSystem() override;

    void Initialize() override;
    void Finalize() override;

    void Update();

private:
    size_t eventSubscriptionId_ = 0;
    std::vector<PlayerExplosionEffectEvent> pendingEvents_;
};
