#pragma once

#include "system/ISystem.h"

/// stl
#include <string>

/// <summary>
/// シーン遷移リクエスト送信システム
/// </summary>
class SceneTransitionRequestSenderSystem
    : public OriGine::ISystem {
public:
    SceneTransitionRequestSenderSystem();
    ~SceneTransitionRequestSenderSystem();

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

protected:
};
