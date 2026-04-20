#include "SpeedModifiers.h"

/// stl
#include <algorithm>

/// ECS
// component
#include "component/physics/Rigidbody.h"

/// math
#include "math/MyEasing.h"
#include <math/Interpolation.h>

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

SpeedModifiers::SpeedModifiers() {}
SpeedModifiers::~SpeedModifiers() {}

void SpeedModifiers::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}

void SpeedModifiers::Finalize() {}

void SpeedModifiers::Edit(Scene* /*_scene*/, EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    CheckBoxCommand("Is Auto Destroyed##" + _parentLabel, isAutoDestroyed_);

    ImGui::Spacing();

    // Trigger Mode
    {
        const char* triggerModeItems[] = {"OnEnter", "WhileColliding"};
        int triggerModeInt             = static_cast<int>(triggerMode_);
        if (ImGui::Combo(("Trigger Mode##" + _parentLabel).c_str(), &triggerModeInt, triggerModeItems, 2)) {
            triggerMode_ = static_cast<SpeedModifierTriggerMode>(triggerModeInt);
        }
    }

    ImGui::Spacing();

    DragGuiCommand("Restore Speed##" + _parentLabel, restoreSpeed_, 0.01f);

    ImGui::Spacing();

    // Axes Space
    {
        const char* axesSpaceItems[] = {"World (X, Y, Z)", "Velocity (Front, Side, Up)"};
        int axesSpaceInt             = static_cast<int>(axesSpace_);
        if (ImGui::Combo(("Axes Space##" + _parentLabel).c_str(), &axesSpaceInt, axesSpaceItems, 2)) {
            axesSpace_ = static_cast<SpeedModifierAxesSpace>(axesSpaceInt);
        }
    }
    const bool isVelocitySpace = (axesSpace_ == SpeedModifierAxesSpace::Velocity);
    const char* axisLabel0     = isVelocitySpace ? "Front" : "X";
    const char* axisLabel1     = isVelocitySpace ? "Side" : "Y";
    const char* axisLabel2     = isVelocitySpace ? "Up" : "Z";

    ImGui::Spacing();

    ImGui::SeparatorText("Additive");
    ImGui::Spacing();

    EasingComboGui("Additive Lerp Ease Type##" + _parentLabel, additiveLerpEaseType_);
    ImGui::Spacing();

    DragGuiCommand("Additive Target##" + _parentLabel, additiveTarget_, 0.01f);
    DragGuiCommand("Additive Duration##" + _parentLabel, additiveDuration_, 0.01f);
    DragGuiCommand("Additive Lerp Duration##" + _parentLabel, additiveFadeInDuration_, 0.01f);
    DragGuiCommand("Additive FadeOut Duration##" + _parentLabel, additiveFadeOutDuration_, 0.01f);
    EasingComboGui("Additive FadeOut Ease Type##" + _parentLabel, additiveFadeOutEaseType_);
    ImGui::Spacing();
    ImGui::Text("Additive Axes");
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel0) + "##AddAxis0" + _parentLabel, additiveAxes_[0]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel1) + "##AddAxis1" + _parentLabel, additiveAxes_[1]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel2) + "##AddAxis2" + _parentLabel, additiveAxes_[2]);

    ImGui::Spacing();

    ImGui::SeparatorText("Multiplier");
    ImGui::Spacing();
    EasingComboGui("Multiplier Lerp Ease Type##" + _parentLabel, multiplierLerpEaseType_);
    ImGui::Spacing();

    DragGuiCommand("Multiplier Target##" + _parentLabel, multiplierTarget_, 0.01f);
    DragGuiCommand("Multiplier Duration##" + _parentLabel, multiplierDuration_, 0.01f);
    DragGuiCommand("Multiplier Lerp Duration##" + _parentLabel, multiplierFadeInDuration_, 0.01f);
    DragGuiCommand("Multiplier FadeOut Duration##" + _parentLabel, multiplierFadeOutDuration_, 0.01f);
    EasingComboGui("Multiplier FadeOut Ease Type##" + _parentLabel, multiplierFadeOutEaseType_);
    ImGui::Spacing();
    ImGui::Text("Multiplier Axes");
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel0) + "##MulAxis0" + _parentLabel, multiplierAxes_[0]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel1) + "##MulAxis1" + _parentLabel, multiplierAxes_[1]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel2) + "##MulAxis2" + _parentLabel, multiplierAxes_[2]);
#endif // _DEBUG
}

void SpeedModifiers::Reset() {
    additiveTarget_          = 0.f;
    additiveDuration_        = 0.0f;
    additiveTimer_           = 0.0f;
    additiveFadeInDuration_  = 0.0f;
    additiveFadeInTimer_     = 0.0f;
    additiveFadeOutDuration_ = 0.0f;
    additiveFadeOutTimer_    = 0.0f;
    additiveAxes_            = {true, true, true};

    multiplierTarget_          = 0.f;
    multiplierDuration_        = 0.0f;
    multiplierTimer_           = 0.0f;
    multiplierFadeInDuration_  = 0.0f;
    multiplierFadeInTimer_     = 0.0f;
    multiplierFadeOutDuration_ = 0.0f;
    multiplierFadeOutTimer_    = 0.0f;
    multiplierAxes_            = {true, true, true};

    axesSpace_   = SpeedModifierAxesSpace::World;
    triggerMode_ = SpeedModifierTriggerMode::OnEnter;
}

void SpeedModifiers::StartAdditiveEffect(
    float _target,
    float _lerpDuration,
    float _effectDuration,
    OriGine::EaseType _easeType,
    float _beforeSpeed,
    float _restoreSpeed,
    float _fadeOutDuration,
    OriGine::EaseType _fadeOutEaseType) {
    additiveTarget_          = _target;
    additiveDuration_        = _effectDuration;
    additiveTimer_           = 0.0f;
    additiveFadeInDuration_  = _lerpDuration;
    additiveFadeInTimer_     = 0.0f;
    additiveFadeOutDuration_ = _fadeOutDuration;
    additiveFadeOutTimer_    = 0.0f;
    additiveLerpEaseType_    = _easeType;
    additiveFadeOutEaseType_ = _fadeOutEaseType;

    beforeSpeed_  = _beforeSpeed;
    restoreSpeed_ = _restoreSpeed;
}

void SpeedModifiers::StartMultiplierEffect(
    float _target,
    float _lerpDuration,
    float _effectDuration,
    OriGine::EaseType _easeType,
    float _beforeSpeed,
    float _restoreSpeed,
    float _fadeOutDuration,
    OriGine::EaseType _fadeOutEaseType) {
    multiplierTarget_          = _target;
    multiplierDuration_        = _effectDuration;
    multiplierTimer_           = 0.0f;
    multiplierFadeInDuration_  = _lerpDuration;
    multiplierFadeInTimer_     = 0.0f;
    multiplierFadeOutDuration_ = _fadeOutDuration;
    multiplierFadeOutTimer_    = 0.0f;
    multiplierLerpEaseType_    = _easeType;
    multiplierFadeOutEaseType_ = _fadeOutEaseType;

    beforeSpeed_  = _beforeSpeed;
    restoreSpeed_ = _restoreSpeed;
}

void to_json(nlohmann::json& _j, const SpeedModifiers& _c) {
    _j = nlohmann::json{
        {"isAutoDestroyed", _c.isAutoDestroyed_},
        {"triggerMode", static_cast<int>(_c.triggerMode_)},
        {"axesSpace", static_cast<int>(_c.axesSpace_)},
        {"additiveTarget", _c.additiveTarget_},
        {"additiveDuration", _c.additiveDuration_},
        {"additiveFadeInDuration", _c.additiveFadeInDuration_},
        {"additiveLerpEaseType", static_cast<int>(_c.additiveLerpEaseType_)},
        {"additiveFadeOutDuration", _c.additiveFadeOutDuration_},
        {"additiveFadeOutEaseType", static_cast<int>(_c.additiveFadeOutEaseType_)},
        {"additiveAxisX", _c.additiveAxes_[0]},
        {"additiveAxisY", _c.additiveAxes_[1]},
        {"additiveAxisZ", _c.additiveAxes_[2]},
        {"multiplierTarget", _c.multiplierTarget_},
        {"multiplierDuration", _c.multiplierDuration_},
        {"multiplierFadeInDuration", _c.multiplierFadeInDuration_},
        {"multiplierLerpEaseType", static_cast<int>(_c.multiplierLerpEaseType_)},
        {"multiplierFadeOutDuration", _c.multiplierFadeOutDuration_},
        {"multiplierFadeOutEaseType", static_cast<int>(_c.multiplierFadeOutEaseType_)},
        {"multiplierAxisX", _c.multiplierAxes_[0]},
        {"multiplierAxisY", _c.multiplierAxes_[1]},
        {"multiplierAxisZ", _c.multiplierAxes_[2]},
        {"restoreSpeed", _c.restoreSpeed_}};
}

void from_json(const nlohmann::json& _j, SpeedModifiers& _c) {
    _j.at("isAutoDestroyed").get_to(_c.isAutoDestroyed_);
    _c.triggerMode_ = static_cast<SpeedModifierTriggerMode>(_j.value("triggerMode", 0));
    _c.axesSpace_   = static_cast<SpeedModifierAxesSpace>(_j.value("axesSpace", 0));
    _j.at("additiveTarget").get_to(_c.additiveTarget_);
    _j.at("additiveDuration").get_to(_c.additiveDuration_);
    _j.at("additiveFadeInDuration").get_to(_c.additiveFadeInDuration_);
    int additiveEaseType;
    _j.at("additiveLerpEaseType").get_to(additiveEaseType);
    _c.additiveLerpEaseType_    = static_cast<OriGine::EaseType>(additiveEaseType);
    _c.additiveFadeOutDuration_ = _j.value("additiveFadeOutDuration", 0.f);
    _c.additiveFadeOutEaseType_ = static_cast<OriGine::EaseType>(_j.value("additiveFadeOutEaseType", 0));
    _c.additiveAxes_[0]         = _j.value("additiveAxisX", true);
    _c.additiveAxes_[1]         = _j.value("additiveAxisY", true);
    _c.additiveAxes_[2]         = _j.value("additiveAxisZ", true);
    _j.at("multiplierTarget").get_to(_c.multiplierTarget_);
    _j.at("multiplierDuration").get_to(_c.multiplierDuration_);
    _j.at("multiplierFadeInDuration").get_to(_c.multiplierFadeInDuration_);
    int multiplierEaseType;
    _j.at("multiplierLerpEaseType").get_to(multiplierEaseType);
    _c.multiplierLerpEaseType_    = static_cast<OriGine::EaseType>(multiplierEaseType);
    _c.multiplierFadeOutDuration_ = _j.value("multiplierFadeOutDuration", 0.f);
    _c.multiplierFadeOutEaseType_ = static_cast<OriGine::EaseType>(_j.value("multiplierFadeOutEaseType", 0));
    _c.multiplierAxes_[0]         = _j.value("multiplierAxisX", true);
    _c.multiplierAxes_[1]         = _j.value("multiplierAxisY", true);
    _c.multiplierAxes_[2]         = _j.value("multiplierAxisZ", true);
    _j.at("restoreSpeed").get_to(_c.restoreSpeed_);
}
