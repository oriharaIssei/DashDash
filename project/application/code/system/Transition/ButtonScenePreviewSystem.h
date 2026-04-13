#pragma once
#include "system/ISystem.h"

/// <summary>
/// ボタングループで指定されているボタンのシーンプレビューを行うシステム
/// </summary>
class ButtonScenePreviewSystem
    : public OriGine::ISystem {
public:
    ButtonScenePreviewSystem();
    ~ButtonScenePreviewSystem();

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
