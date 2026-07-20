#pragma once
#include "system/ISystem.h"

/// <summary>
/// Playerの速度を 3dUIに適応するクラス
/// </summary>
class PlayerSpeedFor3dUI
    : public OriGine::ISystem {
public:
    PlayerSpeedFor3dUI();
    ~PlayerSpeedFor3dUI() override;

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
    /// Playerの現在速度を桁ごとの数字に分解し、3dUI上の各桁MaterialのUVへ反映する
    /// </summary>
    /// <param name="_handle">SpeedFor3dUIComponentを持つエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
