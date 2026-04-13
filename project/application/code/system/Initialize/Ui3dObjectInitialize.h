#pragma once
#include "system/ISystem.h"

/// <summary>
/// UI 3Dオブジェクト初期化システム
/// </summary>
class Ui3dObjectInitialize
    : public OriGine::ISystem {
public:
    Ui3dObjectInitialize();
    ~Ui3dObjectInitialize() override = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// エンティティ更新
    /// </summary>
    /// <param name="_entity">エンティティ</param>
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
