#pragma once
#include "system/ISystem.h"

/// ECS
// component
#include "component/ComponentHandle.h"

/// config
#include "component/ui/UIConfig.h"

/// <summary>
/// シーン遷移システム(エフェクトの再生管理など)
/// </summary>
class SceneTransition
    : public OriGine::ISystem {
public:
    SceneTransition();
    ~SceneTransition() override;

    virtual void Initialize();
    // virtual void Update();
    // virtual void Edit();
    virtual void Finalize();

    virtual void Update() override;

protected:
    /// <summary>
    /// SceneChangerがシーン変更を検知したかどうかを監視し、検知した場合は
    /// ExitSceneUpdateへ遷移するための状態を設定する。
    /// </summary>
    virtual void UpdateEntity(const OriGine::EntityHandle& _handle) override;

    /// <summary>
    /// シーン開始時のDissolveエフェクトを再生し、遷移時間が経過し終えたら再生を停止する。
    /// </summary>
    void EnterSceneUpdate();
    /// <summary>
    /// シーン終了時のDissolveエフェクトを再生し、最大遷移時間に達したらSceneManagerでシーンを切り替える。
    /// </summary>
    void ExitSceneUpdate();

protected:
    // シーン開始演出(EnterSceneUpdate)を実行中か
    bool enterScene_ = true;
    // シーン終了演出(ExitSceneUpdate)を実行中か
    bool exitScene_  = false;

    const float maxTransitionTime_ = AppConfig::UI::kMaxSceneTransitionTime; // 最大遷移時間
    float currentTransitionTime_   = 0.0f; // 現在の遷移時間

    // 現在演出中のEnterScene/対象Entityのハンドル
    OriGine::EntityHandle usingEntityHandle_              = OriGine::EntityHandle();
    // 遷移先シーン情報を持つSceneChangerのハンドル
    OriGine::ComponentHandle sceneChangerComponentHandle_ = OriGine::ComponentHandle();
};
