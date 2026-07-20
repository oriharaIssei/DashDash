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

    /// <summary>
    /// GamefailedEventの購読を開始する。
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// GamefailedEventの購読を解除する。
    /// </summary>
    void Finalize() override;

protected:
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

protected:
    // GamefailedEvent購読解除用のID
    size_t gameFailedEventId_ = 0;
    // ゲーム失敗イベントを受信済みか(1度だけ起動処理を行うためのフラグ)
    bool isLaunched_          = false;
};
