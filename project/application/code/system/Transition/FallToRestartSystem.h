#pragma once
#include "system/Transition/FallDetectionSystem.h"

/// <summary>
/// 落下時にリスタートイベントを発行するシステム
/// ステージセレクトシーンで使用する。
/// </summary>
class FallToRestartSystem final
    : public FallDetectionSystem {
public:
    FallToRestartSystem()           = default;
    ~FallToRestartSystem() override = default;

protected:
    void OnFall() override;
};
