#pragma once

#include "system/ISystem.h"

/// stl
#include <memory>

/// System
#include "system/initialize/CreateSpriteFromTimer.h"

/// <summary>
/// ゲーム開始のタイマー初期化システム
/// </summary>
class StartTimerInitialize
    : public OriGine::ISystem {
public:
    StartTimerInitialize();
    ~StartTimerInitialize();

    /// <summary>
    /// 内部で使用するCreateSpriteFromTimerシステムを生成・初期化する
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// TimerComponentのアニメーションを、対応するタイマー表示用Spriteそれぞれに複製・設定する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

private:
    std::unique_ptr<CreateSpriteFromTimer> createSpriteFromTimerSystem_ = nullptr;
};
