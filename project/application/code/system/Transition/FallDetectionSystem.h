#pragma once
#include "system/ISystem.h"

/// <summary>
/// 落下判定システム (基底)
/// 落下を検出したとき OnFall() を呼ぶ。
/// 発火するイベントは派生クラスで定義する。
/// </summary>
class FallDetectionSystem
    : public OriGine::ISystem {
public:
    FallDetectionSystem();
    ~FallDetectionSystem() override;

    virtual void Initialize() override {}
    virtual void Finalize() override {}

protected:
    /// <summary>
    /// 落下検出の処理。Playerのみを対象とするため、UpdateEntityではなく、PlayerのEntityを直接取得して処理する。
    /// </summary>
    void Update();

    /// <summary>
    /// 落下検出時に呼ばれる。派生クラスでイベントを発火する。
    /// </summary>
    virtual void OnFall() = 0;
};
