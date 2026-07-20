#pragma once

#include "component/IComponent.h"

/// stl
#include <set>

/// <summary>
/// DeltaTimeにScaleをかけるギミックのコンポーネント
/// </summary>
class TimeScaleEffectComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const TimeScaleEffectComponent& _component);
    friend void from_json(const nlohmann::json& _j, TimeScaleEffectComponent& _component);

public:
    TimeScaleEffectComponent();
    ~TimeScaleEffectComponent() override;
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;

    bool IsActive() const { return isActive_; }
    void SetActive(bool _active) { isActive_ = _active; }
    const std::set<std::string>& GetTimeScaleTags() const { return timeScaleTags_; }
    float GetTimeScale() const { return timeScale_; }
    float GetTimeScaleDuration() const { return timeScaleDuration_; }

private:
    bool isActive_{false}; // 効果が発動中かどうか
    std::set<std::string> timeScaleTags_{}; // 効果の対象を絞り込むためのタグ集合
    float timeScale_{1.0f}; // DeltaTimeに乗算するスケール値
    float timeScaleDuration_{0.0f}; // タイムスケールの効果時間
};
