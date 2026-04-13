#pragma once

#include "system/ISystem.h"

/// stl
#include <string>
#include <unordered_map>

/// ECS
// entity
#include "entity/EntityHandle.h"

/// <summary>
/// タイムスケールタイマー
/// </summary>
struct TimeScaleTimer {
    float timeScale  = 0.f; // タイムスケール
    float scaleTimer = 0.f; // どれだけ時間を止めるか
};

/// <summary>
/// タイムスケールエフェクトを処理するシステム
/// </summary>
class TimeScaleEffectSystem
    : public OriGine::ISystem {
public:
    TimeScaleEffectSystem();
    ~TimeScaleEffectSystem() override;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
    void Update() override;

    /// <summary>
    /// タイムスケールタイマーの更新
    /// </summary>
    void UpdateScaleTimer();

private:
    float fadeInTime_        = 0.2f;
    float fadeOutTime_       = 0.6f;
    float effectElapsedTime_ = 0.f;
    OriGine::EntityHandle grayScaleEffectEntityHandle_;
    std::unordered_map<std::string, TimeScaleTimer> stopTimesByTimeScale_;
};
