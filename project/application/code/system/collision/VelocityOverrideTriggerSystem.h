#pragma once

#include "system/ISystem.h"

/// <summary>
/// 衝突した相手に強制移動を適応するシステム
/// </summary>
class VelocityOverrideTriggerSystem
    : public OriGine::ISystem {
public:
    VelocityOverrideTriggerSystem();
    ~VelocityOverrideTriggerSystem() override;

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
    /// VelocityOverrideComponentを持つEntityに衝突（Enter）した相手へ、強制移動速度を適用するイベントを発行する
    /// </summary>
    /// <param name="_handle">VelocityOverrideComponentを持つエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
