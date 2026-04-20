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
    currentOffset_       = idleParams_.offset;
    currentTargetOffset_ = idleParams_.targetOffset;

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
        editStateParams("Idle", idleParams_);
        editStateParams("Dash", dashParams_);
        editStateParams("WallRun", wallRunParams_);
        DragGuiCommand("MinTargetOffsetXOnWallRun##" + _parentLabel, minTargetOffsetXOnWallRun_, 0.01f, 0.f, wallRunParams_.targetOffset[X]);
        DragGuiCommand("tiltDotOnFollow##" + _parentLabel, tiltDotOnFollow_, 0.01f);
        DragGuiCommand("maxRollAngleOnFollow##" + _parentLabel, maxRollAngleOnFollow_, 0.01f);
        ImGui::TreePop();
    }

    ImGui::Spacing();

    DragGuiCommand("rotateSpeedPadStick##" + _parentLabel, rotateSpeedPadStick_, 0.01f);
    DragGuiCommand("rotateSpeedMouse##" + _parentLabel, rotateSpeedMouse_, 0.01f);

    DragGuiCommand("rotateSensitivity##" + _parentLabel, rotateSensitivity_, 0.01f);

    ImGui::Spacing();

    DragGuiCommand("maxRotateX##" + _parentLabel, maxRotateX_, 0.01f);
    DragGuiCommand("minRotateX##" + _parentLabel, minRotateX_, 0.01f);

    ImGui::Spacing();

    label = "Fov##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        DragGuiCommand("fovYInterpolate##" + _parentLabel, fovYInterpolate_, 0.001f);
        DragGuiCommand("fovMin##" + _parentLabel, fovMin_, 0.001f);
        DragGuiCommand("fovMax##" + _parentLabel, fovMax_, 0.001f);
        DragGuiCommand("fovMinSpeed##" + _parentLabel, fovMinSpeed_, 0.1f);
        DragGuiCommand("fovMaxSpeed##" + _parentLabel, fovMaxSpeed_, 0.1f);
        EasingComboGui("fovEaseType##" + _parentLabel, fovEaseType_);
        ImGui::TreePop();
    }

#endif // _DEBUG
}

void CameraController::Finalize() {
}

float CameraController::CalculateFovYBySpeed(float _xzSpeed) const {
    if (fovMaxSpeed_ <= fovMinSpeed_) {
        return fovMin_;
    }
    float t      = std::clamp((_xzSpeed - fovMinSpeed_) / (fovMaxSpeed_ - fovMinSpeed_), 0.f, 1.f);
    float easedT = EasingFunctions[static_cast<int>(fovEaseType_)](t);
    return std::lerp(fovMin_, fovMax_, easedT);
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
    stateParamsToJson(idleJ, _c.idleParams_);
    stateParamsToJson(dashJ, _c.dashParams_);
    stateParamsToJson(wallRunJ, _c.wallRunParams_);

    _j["idleParams"]                = idleJ;
    _j["dashParams"]                = dashJ;
    _j["wallRunParams"]             = wallRunJ;
    _j["minTargetOffsetXOnWallRun"] = _c.minTargetOffsetXOnWallRun_;
    _j["tiltDotOnFollow"]           = _c.tiltDotOnFollow_;
    _j["maxRollAngleOnFollow"]      = _c.maxRollAngleOnFollow_;
    _j["rotateSpeedPadStick"]       = _c.rotateSpeedPadStick_;
    _j["rotateSpeedMouse"]          = _c.rotateSpeedMouse_;
    _j["rotateSensitivity"]         = _c.rotateSensitivity_;
    _j["maxRotateX"]                = _c.maxRotateX_;
    _j["minRotateX"]                = _c.minRotateX_;
    _j["fovYInterpolate"]           = _c.fovYInterpolate_;
    _j["fovMin"]                    = _c.fovMin_;
    _j["fovMax"]                    = _c.fovMax_;
    _j["fovMinSpeed"]               = _c.fovMinSpeed_;
    _j["fovMaxSpeed"]               = _c.fovMaxSpeed_;
    _j["fovEaseType"]               = static_cast<int>(_c.fovEaseType_);
    _j["fixForForwardSpeed"]        = _c.fixForForwardSpeed_;
}

void from_json(const nlohmann::json& _j, CameraController& _c) {
    stateParamsFromJson(_j.at("idleParams"), _c.idleParams_);
    stateParamsFromJson(_j.at("dashParams"), _c.dashParams_);
    stateParamsFromJson(_j.at("wallRunParams"), _c.wallRunParams_);
    _j.at("minTargetOffsetXOnWallRun").get_to(_c.minTargetOffsetXOnWallRun_);
    _c.tiltDotOnFollow_      = _j.value("tiltDotOnFollow", 0.f);
    _c.maxRollAngleOnFollow_ = _j.value("maxRollAngleOnFollow", 0.f);

    _j.at("rotateSpeedPadStick").get_to(_c.rotateSpeedPadStick_);
    _j.at("rotateSpeedMouse").get_to(_c.rotateSpeedMouse_);
    _j.at("rotateSensitivity").get_to(_c.rotateSensitivity_);

    _j.at("maxRotateX").get_to(_c.maxRotateX_);
    _j.at("minRotateX").get_to(_c.minRotateX_);
    _j.at("fovYInterpolate").get_to(_c.fovYInterpolate_);
    _c.fovMin_      = _j.value("fovMin", AppConfig::Camera::kDefaultFovMin);
    _c.fovMax_      = _j.value("fovMax", AppConfig::Camera::kDefaultFovMax);
    _c.fovMinSpeed_ = _j.value("fovMinSpeed", AppConfig::Camera::kDefaultFovMinSpeed);
    _c.fovMaxSpeed_ = _j.value("fovMaxSpeed", AppConfig::Camera::kDefaultFovMaxSpeed);
    _c.fovEaseType_ = static_cast<OriGine::EaseType>(_j.value("fovEaseType", 0));
    _j.at("fixForForwardSpeed").get_to(_c.fixForForwardSpeed_);
}
