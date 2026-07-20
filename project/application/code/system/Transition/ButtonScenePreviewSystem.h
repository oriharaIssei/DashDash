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
    /// <summary>
    /// ButtonGroupで現在選択されているボタンに対応するSubSceneのみをアクティブにする。
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
