#pragma once

#include "system/ISystem.h"

/// <summary>
/// ゲーム失敗メッセージを受け取ってゲーム失敗シーンを起動するシステム
/// </summary>
class GameFailedSceneLaunchSystem
    : public OriGine::ISystem {
public:
    GameFailedSceneLaunchSystem();
    ~GameFailedSceneLaunchSystem() override;

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

protected:
    size_t gameFailedEventId_ = 0;
    bool isLaunched_          = false;
};
