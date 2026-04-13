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

    CheckBoxCommand("Is Auto Destroyed##" + _parentLabel, isAutoDestroyed);

    ImGui::Spacing();

    // Trigger Mode
    {
        const char* triggerModeItems[] = {"OnEnter", "WhileColliding"};
        int triggerModeInt             = static_cast<int>(triggerMode);
        if (ImGui::Combo(("Trigger Mode##" + _parentLabel).c_str(), &triggerModeInt, triggerModeItems, 2)) {
            triggerMode = static_cast<TriggerMode>(triggerModeInt);
        }
    }

    ImGui::Spacing();

    DragGuiCommand("Restore Speed##" + _parentLabel, restoreSpeed, 0.01f);

    ImGui::Spacing();

    // Axes Space
    {
        const char* axesSpaceItems[] = {"World (X, Y, Z)", "Velocity (Front, Side, Up)"};
        int axesSpaceInt             = static_cast<int>(axesSpace);
        if (ImGui::Combo(("Axes Space##" + _parentLabel).c_str(), &axesSpaceInt, axesSpaceItems, 2)) {
            axesSpace = static_cast<AxesSpace>(axesSpaceInt);
        }
    }
    const bool isVelocitySpace = (axesSpace == AxesSpace::Velocity);
    const char* axisLabel0     = isVelocitySpace ? "Front" : "X";
    const char* axisLabel1     = isVelocitySpace ? "Side" : "Y";
    const char* axisLabel2     = isVelocitySpace ? "Up" : "Z";

    ImGui::Spacing();

    ImGui::SeparatorText("Additive");
    ImGui::Spacing();

    EasingComboGui("Additive Lerp Ease Type##" + _parentLabel, additiveLerpEaseType);
    ImGui::Spacing();

    DragGuiCommand("Additive Target##" + _parentLabel, additiveTarget, 0.01f);
    DragGuiCommand("Additive Duration##" + _parentLabel, additiveDuration, 0.01f);
    DragGuiCommand("Additive Lerp Duration##" + _parentLabel, additiveFadeInDuration, 0.01f);
    DragGuiCommand("Additive FadeOut Duration##" + _parentLabel, additiveFadeOutDuration, 0.01f);
    EasingComboGui("Additive FadeOut Ease Type##" + _parentLabel, additiveFadeOutEaseType);
    ImGui::Spacing();
    ImGui::Text("Additive Axes");
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel0) + "##AddAxis0" + _parentLabel, additiveAxes[0]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel1) + "##AddAxis1" + _parentLabel, additiveAxes[1]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel2) + "##AddAxis2" + _parentLabel, additiveAxes[2]);

    ImGui::Spacing();

    ImGui::SeparatorText("Multiplier");
    ImGui::Spacing();
    EasingComboGui("Multiplier Lerp Ease Type##" + _parentLabel, multiplierLerpEaseType);
    ImGui::Spacing();

    DragGuiCommand("Multiplier Target##" + _parentLabel, multiplierTarget, 0.01f);
    DragGuiCommand("Multiplier Duration##" + _parentLabel, multiplierDuration, 0.01f);
    DragGuiCommand("Multiplier Lerp Duration##" + _parentLabel, multiplierFadeInDuration, 0.01f);
    DragGuiCommand("Multiplier FadeOut Duration##" + _parentLabel, multiplierFadeOutDuration, 0.01f);
    EasingComboGui("Multiplier FadeOut Ease Type##" + _parentLabel, multiplierFadeOutEaseType);
    ImGui::Spacing();
    ImGui::Text("Multiplier Axes");
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel0) + "##MulAxis0" + _parentLabel, multiplierAxes[0]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel1) + "##MulAxis1" + _parentLabel, multiplierAxes[1]);
    ImGui::SameLine();
    CheckBoxCommand(std::string(axisLabel2) + "##MulAxis2" + _parentLabel, multiplierAxes[2]);
#endif // _DEBUG
}

void SpeedModifiers::Reset() {
    additiveTarget          = 0.f;
    additiveDuration        = 0.0f;
    additiveTimer           = 0.0f;
    additiveFadeInDuration    = 0.0f;
    additiveFadeInTimer     = 0.0f;
    additiveFadeOutDuration = 0.0f;
    additiveFadeOutTimer    = 0.0f;
    additiveAxes            = {true, true, true};

    multiplierTarget          = 0.f;
    multiplierDuration        = 0.0f;
    multiplierTimer           = 0.0f;
    multiplierFadeInDuration    = 0.0f;
    multiplierFadeInTimer     = 0.0f;
    multiplierFadeOutDuration = 0.0f;
    multiplierFadeOutTimer    = 0.0f;
    multiplierAxes            = {true, true, true};

    axesSpace   = AxesSpace::World;
    triggerMode = TriggerMode::OnEnter;
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
    additiveTarget          = _target;
    additiveDuration        = _effectDuration;
    additiveTimer           = 0.0f;
    additiveFadeInDuration    = _lerpDuration;
    additiveFadeInTimer     = 0.0f;
    additiveFadeOutDuration = _fadeOutDuration;
    additiveFadeOutTimer    = 0.0f;
    additiveLerpEaseType    = _easeType;
    additiveFadeOutEaseType = _fadeOutEaseType;

    beforeSpeed  = _beforeSpeed;
    restoreSpeed = _restoreSpeed;
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
    multiplierTarget          = _target;
    multiplierDuration        = _effectDuration;
    multiplierTimer           = 0.0f;
    multiplierFadeInDuration    = _lerpDuration;
    multiplierFadeInTimer     = 0.0f;
    multiplierFadeOutDuration = _fadeOutDuration;
    multiplierFadeOutTimer    = 0.0f;
    multiplierLerpEaseType    = _easeType;
    multiplierFadeOutEaseType = _fadeOutEaseType;

    beforeSpeed  = _beforeSpeed;
    restoreSpeed = _restoreSpeed;
}

void to_json(nlohmann::json& _j, const SpeedModifiers& _c) {
    _j = nlohmann::json{
        {"isAutoDestroyed", _c.isAutoDestroyed},
        {"triggerMode", static_cast<int>(_c.triggerMode)},
        {"axesSpace", static_cast<int>(_c.axesSpace)},
        {"additiveTarget", _c.additiveTarget},
        {"additiveDuration", _c.additiveDuration},
        {"additiveFadeInDuration", _c.additiveFadeInDuration},
        {"additiveLerpEaseType", static_cast<int>(_c.additiveLerpEaseType)},
        {"additiveFadeOutDuration", _c.additiveFadeOutDuration},
        {"additiveFadeOutEaseType", static_cast<int>(_c.additiveFadeOutEaseType)},
        {"additiveAxisX", _c.additiveAxes[0]},
        {"additiveAxisY", _c.additiveAxes[1]},
        {"additiveAxisZ", _c.additiveAxes[2]},
        {"multiplierTarget", _c.multiplierTarget},
        {"multiplierDuration", _c.multiplierDuration},
        {"multiplierFadeInDuration", _c.multiplierFadeInDuration},
        {"multiplierLerpEaseType", static_cast<int>(_c.multiplierLerpEaseType)},
        {"multiplierFadeOutDuration", _c.multiplierFadeOutDuration},
        {"multiplierFadeOutEaseType", static_cast<int>(_c.multiplierFadeOutEaseType)},
        {"multiplierAxisX", _c.multiplierAxes[0]},
        {"multiplierAxisY", _c.multiplierAxes[1]},
        {"multiplierAxisZ", _c.multiplierAxes[2]},
        {"restoreSpeed", _c.restoreSpeed}};
}

void from_json(const nlohmann::json& _j, SpeedModifiers& _c) {
    _j.at("isAutoDestroyed").get_to(_c.isAutoDestroyed);
    _c.triggerMode = static_cast<SpeedModifiers::TriggerMode>(_j.value("triggerMode", 0));
    _c.axesSpace   = static_cast<SpeedModifiers::AxesSpace>(_j.value("axesSpace", 0));
    _j.at("additiveTarget").get_to(_c.additiveTarget);
    _j.at("additiveDuration").get_to(_c.additiveDuration);
    _j.at("additiveFadeInDuration").get_to(_c.additiveFadeInDuration);
    int additiveEaseType;
    _j.at("additiveLerpEaseType").get_to(additiveEaseType);
    _c.additiveLerpEaseType    = static_cast<OriGine::EaseType>(additiveEaseType);
    _c.additiveFadeOutDuration = _j.value("additiveFadeOutDuration", 0.f);
    _c.additiveFadeOutEaseType = static_cast<OriGine::EaseType>(_j.value("additiveFadeOutEaseType", 0));
    _c.additiveAxes[0]         = _j.value("additiveAxisX", true);
    _c.additiveAxes[1]         = _j.value("additiveAxisY", true);
    _c.additiveAxes[2]         = _j.value("additiveAxisZ", true);
    _j.at("multiplierTarget").get_to(_c.multiplierTarget);
    _j.at("multiplierDuration").get_to(_c.multiplierDuration);
    _j.at("multiplierFadeInDuration").get_to(_c.multiplierFadeInDuration);
    int multiplierEaseType;
    _j.at("multiplierLerpEaseType").get_to(multiplierEaseType);
    _c.multiplierLerpEaseType    = static_cast<OriGine::EaseType>(multiplierEaseType);
    _c.multiplierFadeOutDuration = _j.value("multiplierFadeOutDuration", 0.f);
    _c.multiplierFadeOutEaseType = static_cast<OriGine::EaseType>(_j.value("multiplierFadeOutEaseType", 0));
    _c.multiplierAxes[0]         = _j.value("multiplierAxisX", true);
    _c.multiplierAxes[1]         = _j.value("multiplierAxisY", true);
    _c.multiplierAxes[2]         = _j.value("multiplierAxisZ", true);
    _j.at("restoreSpeed").get_to(_c.restoreSpeed);
}
