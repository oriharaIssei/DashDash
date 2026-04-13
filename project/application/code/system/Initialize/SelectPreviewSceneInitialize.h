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

    void Initialize();
    void Finalize();

protected:
    void UpdateEntity(OriGine::EntityHandle _handle);
};
