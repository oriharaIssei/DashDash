#pragma once
#include "system/ISystem.h"

/// <summary>
/// Playerが走っているときにエフェクトを発生させるシステム
/// </summary>
class EffectOnPlayerRun
    : public OriGine::ISystem {
public:
    EffectOnPlayerRun();
    ~EffectOnPlayerRun();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// Playerの走行状態に応じて、タイヤの回転/傾き・trailやBackFireの色/強さ・スピードラインエフェクトを更新する
    /// </summary>
    /// <param name="_entity">対象のPlayerエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _entity) override;

private:
    const float kMaxIntensity_         = 3.8f; // スピードラインエフェクトの最大強度
    const float kThresholdSpeedForSpeedWave_ = 52.f; // スピードウェーブエフェクトを発生させる速度閾値

};
