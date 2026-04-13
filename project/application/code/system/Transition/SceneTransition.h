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
    virtual void UpdateEntity(OriGine::EntityHandle _handle) override;

    void EnterSceneUpdate();
    void ExitSceneUpdate();

protected:
    bool enterScene_ = true;
    bool exitScene_  = false;

    const float maxTransitionTime_ = AppConfig::UI::kMaxSceneTransitionTime; // 最大遷移時間
    float currentTransitionTime_   = 0.0f; // 現在の遷移時間

    OriGine::EntityHandle usingEntityHandle_              = OriGine::EntityHandle();
    OriGine::ComponentHandle sceneChangerComponentHandle_ = OriGine::ComponentHandle();
};
