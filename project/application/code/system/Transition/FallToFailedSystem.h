#pragma once
#include "system/Transition/FallDetectionSystem.h"

/// <summary>
/// 落下時にゲーム失敗イベントを発行するシステム
/// ゲームシーンで使用する。
/// </summary>
class FallToFailedSystem final : public FallDetectionSystem {
public:
    FallToFailedSystem()           = default;
    ~FallToFailedSystem() override = default;

protected:
    void OnFall() override;
};
