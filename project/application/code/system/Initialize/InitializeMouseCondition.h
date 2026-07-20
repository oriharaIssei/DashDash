#pragma once
#include "system/ISystem.h"

/// <summary>
/// Initialize時にマウスの状態を指定された状態にするシステム
/// </summary>
class InitializeMouseCondition
    : public OriGine::ISystem {
public:
    InitializeMouseCondition();
    ~InitializeMouseCondition() override = default;

    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// 有効なMouseConditionコンポーネントに従い、カーソル表示・固定座標をマウス入力へ反映する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
