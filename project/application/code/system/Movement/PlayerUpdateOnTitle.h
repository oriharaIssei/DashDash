#pragma once

#include "system/ISystem.h"

/// <summary>
/// タイトル画面でのプレイヤーの更新を行うシステム
/// プレイヤーのジャンプ入力を無効化し、常に地面に接地している状態にする
/// </summary>
class PlayerUpdateOnTitle
    : public OriGine::ISystem {
public:
    PlayerUpdateOnTitle();
    ~PlayerUpdateOnTitle() override;
    void Initialize();
    void Finalize();

private:
    /// <summary>
    /// タイトル画面用のプレイヤー更新処理。ジャンプ無効化・強制接地に加え、
    /// PlayerDashStateに準じたギアレベル上昇・速度更新・入力なし時の減速処理を行う
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
