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

    /// <summary>
    /// 初期化処理。PlayerExplosionEffectEventの購読を開始する
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理。PlayerExplosionEffectEventの購読を解除する
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 受信した爆発イベントを処理し、破片パーツの生成・飛散・カメラシェイクを行う
    /// </summary>
    void Update();

private:
    size_t eventSubscriptionId_ = 0; // MessageBusへの購読ID（Finalize時の解除に使用）
    std::vector<PlayerExplosionEffectEvent> pendingEvents_; // Update()で処理待ちの爆発イベント
};
