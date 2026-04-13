#include "SceneTransition.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

/// ECS
// component
#include "component/effect/post/DissolveEffectParam.h"
#include "component/scene/SceneChanger.h"

/// math
#include "math/MyEasing.h"

using namespace OriGine;

SceneTransition::SceneTransition() : ISystem(SystemCategory::StateTransition) {}
SceneTransition::~SceneTransition() {}

void SceneTransition::Initialize() {
    enterScene_ = true;
    exitScene_  = false;

    currentTransitionTime_ = maxTransitionTime_;
}

void SceneTransition::Finalize() {
    enterScene_                  = false;
    exitScene_                   = false;
    usingEntityHandle_           = EntityHandle();
    sceneChangerComponentHandle_ = ComponentHandle();

    currentTransitionTime_ = 0.f;
}

void SceneTransition::Update() {
#ifdef _DEBUG
    // 計測開始
    deltaTimer_.Initialize();
#endif

    if (entities_.empty()) {
        return;
    }

    EraseDeadEntity();

    // シーンに入るエフェクトの更新
    if (enterScene_) {
        EntityHandle enterSceneEntityHandle = GetUniqueEntity("EnterScene");
        if (!enterSceneEntityHandle.IsValid()) {
            enterScene_ = false;
            return;
        }
        usingEntityHandle_ = enterSceneEntityHandle;

        EnterSceneUpdate();
    } else if (exitScene_) { // シーンから出るエフェクトの更新
        ExitSceneUpdate();
    } else {
        for (auto& entityHandle : entities_) {
            UpdateEntity(entityHandle);
        }
    }

#ifdef _DEBUG
    // 計測終了
    deltaTimer_.Update();
#endif
}

void SceneTransition::UpdateEntity(EntityHandle _handle) {
    auto& sceneChangers = GetComponents<SceneChanger>(_handle);
    if (sceneChangers.empty()) {
        return;
    }

    // シーン遷移の検知

    for (auto& sceneChanger : sceneChangers) {
        if (sceneChanger.isChanged()) {

            currentTransitionTime_       = 0.f;
            exitScene_                   = true;
            enterScene_                  = false;
            usingEntityHandle_           = _handle;
            sceneChangerComponentHandle_ = sceneChanger.GetHandle();
            return;
        }
    }
}

void SceneTransition::EnterSceneUpdate() {

    OriGine::Entity* enterSceneEntity = GetEntity(usingEntityHandle_);
    if (enterSceneEntity == nullptr) {
        return;
    }
    auto& dissolveEffectComps = GetComponents<DissolveEffectParam>(usingEntityHandle_);
    if (dissolveEffectComps.empty()) {
        return;
    }

    currentTransitionTime_ -= Engine::GetInstance()->GetDeltaTime();

    ///====================================================================
    /// 遷移時間が0以下になった場合、再生終了
    ///====================================================================
    if (currentTransitionTime_ <= 0.0f) {
        currentTransitionTime_ = 0.0f;
        enterScene_            = false;
        exitScene_             = false;

        for (auto& dissolveEffectParam : dissolveEffectComps) {
            dissolveEffectParam.Stop();
        }

        return;
    }

    ///====================================================================
    /// 再生中はエフェクトの更新
    ///====================================================================
    for (auto& dissolveEffectParam : dissolveEffectComps) {
        if (!dissolveEffectParam.IsActive()) {
            dissolveEffectParam.Play();
        }
        dissolveEffectParam.SetThreshold(EaseInQuad(currentTransitionTime_ / maxTransitionTime_));
    }
}

void SceneTransition::ExitSceneUpdate() {
    currentTransitionTime_ += Engine::GetInstance()->GetDeltaTime();
    /// ====================================================================
    /// 最大遷移時間を超えた場合、シーン変更を実行
    /// ====================================================================
    if (currentTransitionTime_ >= maxTransitionTime_) {
        currentTransitionTime_ = maxTransitionTime_;
        enterScene_            = false;
        exitScene_             = false;

        // シーン変更を実行
        SceneChanger* sceneChanger = GetComponent<SceneChanger>(sceneChangerComponentHandle_);
        GetScene()->GetSceneManager()->ChangeScene(sceneChanger->GetNextSceneName());
    }

    /// ====================================================================
    /// 再生中はエフェクトの更新
    /// ===================================================================
    OriGine::Entity* enterSceneEntity = GetEntity(usingEntityHandle_);
    if (enterSceneEntity == nullptr) {
        return;
    }

    auto& dissolveEffectComps = GetComponents<DissolveEffectParam>(usingEntityHandle_);
    if (dissolveEffectComps.empty()) {
        return;
    }

    for (auto& dissolveEffectParam : dissolveEffectComps) {

        if (!dissolveEffectParam.IsActive()) {
            dissolveEffectParam.Play();
        }
        dissolveEffectParam.SetThreshold(EaseInQuad(currentTransitionTime_ / maxTransitionTime_));
    }
}
