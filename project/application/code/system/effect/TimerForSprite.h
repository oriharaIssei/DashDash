#pragma once
#include "system/ISystem.h"

/// <summary>
/// Timerの数値をSpriteに適用するシステム
/// </summary>
class TimerForSprite
    : public OriGine::ISystem {
public:
    TimerForSprite();
    ~TimerForSprite() override;

    /// <summary>
    /// 初期化処理
    /// </summary>
    virtual void Initialize();
    /// <summary>
    /// 終了処理
    /// </summary>
    virtual void Finalize();

protected:
    /// <summary>
    /// TimerComponentの数値を桁ごとに分解し、対応するSpriteのテクスチャ座標へ反映する
    /// </summary>
    /// <param name="_handle">TimerComponent/TimerForSpriteComponentを持つエンティティハンドル</param>
    virtual void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
