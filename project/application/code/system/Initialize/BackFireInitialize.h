#pragma once
#include "system/ISystem.h"

/// <summary>
/// BackFireの初期化を行うシステム
/// </summary>
class BackFireInitialize
    : public OriGine::ISystem {
public:
    BackFireInitialize();
    ~BackFireInitialize() override;

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
    /// BackFireとそのSparksをPlayerの子として親子付けする
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
