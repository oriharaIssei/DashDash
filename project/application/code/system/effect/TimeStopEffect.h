#pragma once

#include "system/ISystem.h"

/// <summary>
/// 時間停止エフェクト
/// </summary>
class TimeStopEffect
    : public OriGine::ISystem {
public:
    TimeStopEffect();
    ~TimeStopEffect() override = default;
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// Timerのタイムスケールを見て時間停止中かどうかを判定し、Timer用スプライトの色を切り替える
    /// </summary>
    void Update() override;
};
