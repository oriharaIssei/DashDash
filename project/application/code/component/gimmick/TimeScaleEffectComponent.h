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
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

private:
    bool isActive_{false};
    std::set<std::string> timeScaleTags_{};
    float timeScale_{1.0f};
    float timeScaleDuration_{0.0f}; // タイムスケールの効果時間
public:
    bool IsActive() const { return isActive_; }
    void SetActive(bool _active) { isActive_ = _active; }
    const std::set<std::string>& GetTimeScaleTags() const { return timeScaleTags_; }
    float GetTimeScale() const { return timeScale_; }
    float GetTimeScaleDuration() const { return timeScaleDuration_; }
};
