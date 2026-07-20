#pragma once

#include "system/ISystem.h"

/// <summary>
/// Rail 初期化システム
/// </summary>
class RailInitializeSystem
    : public OriGine::ISystem {
public:
    RailInitializeSystem();
    ~RailInitializeSystem();

    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// RailPointsの制御点からスプライン補間を行い、各セグメントにCapsuleColliderを生成・設定する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
