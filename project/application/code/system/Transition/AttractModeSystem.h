#pragma once

#include "system/ISystem.h"

/// <summary>
/// アトラクションモードのシステム
/// (未入力時間が一定以上になったら、Title画面などのSubSceneを起動する)
/// </summary>
class AttractModeSystem
    : public OriGine::ISystem {
public:
    AttractModeSystem();
    ~AttractModeSystem() override = default;

    void Initialize() override;
    void Finalize() override;

private:
    void Update() override;
    void UpdateEntity(OriGine::EntityHandle _handle) override;

private:
    bool isAttractModeActive_              = true;
    bool isSceneInitializing_              = false; // シーン初期化中かどうか (アトラクションモード開始時にシーンを切り替えるため
    bool previouslyActive_                 = true; // 前フレームのアトラクションモード状態
    const float kAttractModeThreshold_     = 30.0f; // アトラクションモードに入るまでの未入力時間 (秒)
    float idleTimer_                       = 0.0f; // 未入力時間のタイマー
    const float kSceneInitializationDelay_ = 1.0f; // シーン初期化後、アトラクションモードを開始するまでの遅延時間 (秒)
    float sceneInitializationTimer_        = 0.0f; // シーン初期化後のタイマー
private:
    bool IsTriggered() const;
    bool IsReleased() const;
};
