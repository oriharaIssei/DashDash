#pragma once
#include "system/ISystem.h"

/// <summary>
/// プレイヤーをスタート位置に移動させるシステム
/// </summary>
class TakePlayerToStartPosition
    : public OriGine::ISystem {
public:
    TakePlayerToStartPosition();
    ~TakePlayerToStartPosition() override;
    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    virtual void Initialize();
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    virtual void Finalize();

protected:
    /// <summary>
    /// "StartPosition"エンティティの位置を基に、対象エンティティの位置・回転・スケールを初期状態にリセットする
    /// </summary>
    virtual void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
