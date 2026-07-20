#pragma once
#include "system/ISystem.h"

/// <summary>
/// ステージをクリアしたときのタイムを取得するシステム
/// </summary>
class GetClearTime
    : public OriGine::ISystem {
public:
    GetClearTime();
    ~GetClearTime() override;
    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// 直前のプレイでのクリアタイムをPlayerProgressStoreから取得し、Timerコンポーネントへ反映する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
