#pragma once
#include "system/ISystem.h"

/// <summary>
/// ペナルティ時間のスプライトを更新するシステム
/// </summary>
class PenaltyTimeSpriteUpdate
    : public OriGine::ISystem {
public:
    PenaltyTimeSpriteUpdate();
    ~PenaltyTimeSpriteUpdate() override;
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
    /// エンティティの更新処理
    /// </summary>
    void UpdateEntity(OriGine::EntityHandle _handle) override;

private:
    float animationTime_ = 1.2f;
};
