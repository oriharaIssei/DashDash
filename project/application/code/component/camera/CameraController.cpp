#include "CameraController.h"

#ifdef _DEBUG
/// gui
#include "myGui/MyGui.h"
#endif // _DEBUG

/// state
#include "state/CameraIdleState.h"

/// math
#include <numbers>

using namespace OriGine;

void CameraController::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    currentOffset       = idleParams.offset;
    currentTargetOffset = idleParams.targetOffset;

    currentCameraStateType_ = CameraMoveState::IDLE;
}

void CameraController::Edit(Scene* /*_scene*/, EntityHandle /*_OriGine::Entity*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    auto editStateParams = [&](const std::string& _name, CameraStateParams& _params) {
        std::string stateLabel = _name + "##" + _parentLabel;
        if (ImGui::TreeNode(stateLabel.c_str())) {
            DragGuiVectorCommand("targetOffset##" + _name + _parentLabel, _params.targetOffset, 0.01f);
            DragGuiVectorCommand("offset##" + _name + _parentLabel, _params.offset, 0.01f);
            DragGuiVectorCommand("interTargetInterp##" + _name + _parentLabel, _params.interTargetInterpolation, 0.01f);
            DragGuiVectorCommand("interLookAtInterp##" + _name + _parentLabel, _params.interLookAtTargetInterpolation, 0.01f);
            ImGui::TreePop();
        }
    };

    std::string label = "StateParams##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        editStateParams("Idle", idleParams);
        editStateParams("Dash", dashParams);
        editStateParams("WallRun", wallRunParams);
        DragGuiCommand("MinTargetOffsetXOnWallRun##" + _parentLabel, minTargetOffsetXOnWallRun, 0.01f, 0.f, wallRunParams.targetOffset[X]);
        DragGuiCommand("tiltDotOnFollow##" + _parentLabel, tiltDotOnFollow, 0.01f);
        DragGuiCommand("maxRollAngleOnFollow##" + _parentLabel, maxRollAngleOnFollow, 0.01f);
        ImGui::TreePop();
    }

    ImGui::Spacing();

    DragGuiCommand("rotateSpeedPadStick##" + _parentLabel, rotateSpeedPadStick, 0.01f);
    DragGuiCommand("rotateSpeedMouse##" + _parentLabel, rotateSpeedMouse, 0.01f);

    DragGuiCommand("rotateSensitivity##" + _parentLabel, rotateSensitivity, 0.01f);

    ImGui::Spacing();

    DragGuiCommand("maxRotateX##" + _parentLabel, maxRotateX, 0.01f);
    DragGuiCommand("minRotateX##" + _parentLabel, minRotateX, 0.01f);

    ImGui::Spacing();

    label = "Fov##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        DragGuiCommand("fovYInterpolate##" + _parentLabel, fovYInterpolate, 0.001f);
        DragGuiCommand("fovMin##" + _parentLabel, fovMin, 0.001f);
        DragGuiCommand("fovMax##" + _parentLabel, fovMax, 0.001f);
        DragGuiCommand("fovMinSpeed##" + _parentLabel, fovMinSpeed, 0.1f);
        DragGuiCommand("fovMaxSpeed##" + _parentLabel, fovMaxSpeed, 0.1f);
        EasingComboGui("fovEaseType##" + _parentLabel, fovEaseType);
        ImGui::TreePop();
    }

#endif // _DEBUG
}

void CameraController::Finalize() {
}

float CameraController::CalculateFovYBySpeed(float _xzSpeed) const {
    if (fovMaxSpeed <= fovMinSpeed) {
        return fovMin;
    }
    float t      = std::clamp((_xzSpeed - fovMinSpeed) / (fovMaxSpeed - fovMinSpeed), 0.f, 1.f);
    float easedT = EasingFunctions[static_cast<int>(fovEaseType)](t);
    return std::lerp(fovMin, fovMax, easedT);
}

namespace {
void stateParamsToJson(nlohmann::json& _j, const CameraStateParams& _p) {
    _j["targetOffset"]                   = _p.targetOffset;
    _j["offset"]                         = _p.offset;
    _j["interTargetInterpolation"]       = _p.interTargetInterpolation;
    _j["interLookAtTargetInterpolation"] = _p.interLookAtTargetInterpolation;
}

void stateParamsFromJson(const nlohmann::json& _j, CameraStateParams& _p) {
    _j.at("targetOffset").get_to(_p.targetOffset);
    _j.at("offset").get_to(_p.offset);
    _j.at("interTargetInterpolation").get_to(_p.interTargetInterpolation);
    _j.at("interLookAtTargetInterpolation").get_to(_p.interLookAtTargetInterpolation);
}
} // namespace

void to_json(nlohmann::json& _j, const CameraController& _c) {
    nlohmann::json idleJ, dashJ, wallRunJ;
    stateParamsToJson(idleJ, _c.idleParams);
    stateParamsToJson(dashJ, _c.dashParams);
    stateParamsToJson(wallRunJ, _c.wallRunParams);

    _j["idleParams"]                = idleJ;
    _j["dashParams"]                = dashJ;
    _j["wallRunParams"]             = wallRunJ;
    _j["minTargetOffsetXOnWallRun"] = _c.minTargetOffsetXOnWallRun;
    _j["tiltDotOnFollow"]           = _c.tiltDotOnFollow;
    _j["maxRollAngleOnFollow"]      = _c.maxRollAngleOnFollow;
    _j["rotateSpeedPadStick"]       = _c.rotateSpeedPadStick;
    _j["rotateSpeedMouse"]          = _c.rotateSpeedMouse;
    _j["rotateSensitivity"]         = _c.rotateSensitivity;
    _j["maxRotateX"]                = _c.maxRotateX;
    _j["minRotateX"]                = _c.minRotateX;
    _j["fovYInterpolate"]           = _c.fovYInterpolate;
    _j["fovMin"]                    = _c.fovMin;
    _j["fovMax"]                    = _c.fovMax;
    _j["fovMinSpeed"]               = _c.fovMinSpeed;
    _j["fovMaxSpeed"]               = _c.fovMaxSpeed;
    _j["fovEaseType"]               = static_cast<int>(_c.fovEaseType);
    _j["fixForForwardSpeed"]        = _c.fixForForwardSpeed;
}

void from_json(const nlohmann::json& _j, CameraController& _c) {
    stateParamsFromJson(_j.at("idleParams"), _c.idleParams);
    stateParamsFromJson(_j.at("dashParams"), _c.dashParams);
    stateParamsFromJson(_j.at("wallRunParams"), _c.wallRunParams);
    _j.at("minTargetOffsetXOnWallRun").get_to(_c.minTargetOffsetXOnWallRun);
    _c.tiltDotOnFollow      = _j.value("tiltDotOnFollow", 0.f);
    _c.maxRollAngleOnFollow = _j.value("maxRollAngleOnFollow", 0.f);

    _j.at("rotateSpeedPadStick").get_to(_c.rotateSpeedPadStick);
    _j.at("rotateSpeedMouse").get_to(_c.rotateSpeedMouse);
    _j.at("rotateSensitivity").get_to(_c.rotateSensitivity);

    _j.at("maxRotateX").get_to(_c.maxRotateX);
    _j.at("minRotateX").get_to(_c.minRotateX);
    _j.at("fovYInterpolate").get_to(_c.fovYInterpolate);
    _c.fovMin      = _j.value("fovMin", AppConfig::Camera::kDefaultFovMin);
    _c.fovMax      = _j.value("fovMax", AppConfig::Camera::kDefaultFovMax);
    _c.fovMinSpeed = _j.value("fovMinSpeed", AppConfig::Camera::kDefaultFovMinSpeed);
    _c.fovMaxSpeed = _j.value("fovMaxSpeed", AppConfig::Camera::kDefaultFovMaxSpeed);
    _c.fovEaseType = static_cast<OriGine::EaseType>(_j.value("fovEaseType", 0));
    _j.at("fixForForwardSpeed").get_to(_c.fixForForwardSpeed);
}
