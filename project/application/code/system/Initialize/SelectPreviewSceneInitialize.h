#pragma once

#include "system/ISystem.h"

/// <summary>
/// SelectSceneにある、ステージプレビューのシーンを初期化するシステム
/// </summary>
class SelectPreviewSceneInitialize
    : public OriGine::ISystem {
public:
    SelectPreviewSceneInitialize();
    ~SelectPreviewSceneInitialize();

    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize();
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize();

protected:
    /// <summary>
    /// 未読み込みのSubSceneを読み込み、プレビュー用に不要なシステムカテゴリ・UI描画を無効化する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle);
};
