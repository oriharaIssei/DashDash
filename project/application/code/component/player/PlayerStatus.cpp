#include "PlayerStatus.h"

/// component
#include "component/physics/Rigidbody.h"
#include "component/transform/Transform.h"
#include "PlayerInput.h"
#include "State/PlayerState.h"

/// externals
#ifdef _DEBUG
#include "myGui/MyGui.h"
#include <imgui/imgui.h>
#endif // _DEBUG

/// math
#include "math/SpringDamper.h"
#include <math/Interpolation.h>
#include <math/MathEnv.h>
#include <math/Quaternion.h>
#include <math/Sequence.h>

using namespace OriGine;

PlayerStatus::PlayerStatus() {}
PlayerStatus::~PlayerStatus() {}

void PlayerStatus::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    gearUpCoolTime_  = baseGearupCoolTime_;
    currentMaxSpeed_ = baseSpeed_;

    currentWallRunInterval_ = 0.f;
    currentRailInterval_    = 0.f;
}

void PlayerStatus::Edit([[maybe_unused]] Scene* _scene, [[maybe_unused]] EntityHandle _owner, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    DragGuiCommand("Deceleration Factor##" + _parentLabel, decelerationFactor_, 0.01f, 0.f, 1.f);

    ImGui::Spacing();

    std::string label = "Speed##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        DragGuiCommand("baseSpeed##" + _parentLabel, baseSpeed_, 0.01f);
        DragGuiCommand("speedUpRateBase##" + _parentLabel, speedUpRateBase_, 0.01f);
        DragGuiCommand("speedUpRateCommonRate##" + _parentLabel, speedUpRateCommonRate_, 0.01f);

        label = "SpeedByGearLevel##" + _parentLabel;
        if (ImGui::BeginTable(label.c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Gear Level");
            ImGui::TableSetupColumn("Speed");
            ImGui::TableHeadersRow();
            for (int level = 1; level <= kMaxPlayerGearLevel; ++level) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", level);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", CalculateSpeedByGearLevel(level));
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();

        ImGui::SeparatorText("Directional Speed");
        DragGuiVectorCommand<2, float>("baseDirectionalSpeed##" + _parentLabel, baseDirectionalSpeed_, 0.01f);
        DragGuiVectorCommand<2, float>("directionalSpeedUpRateBase##" + _parentLabel, directionalSpeedUpRateBase_, 0.01f);
        DragGuiVectorCommand<2, float>("directionalSpeedUpRateCommonRate##" + _parentLabel, directionalSpeedUpRateCommonRate_, 0.01f);

        label = "DirectionalSpeedByGearLevel##" + _parentLabel;
        if (ImGui::BeginTable(label.c_str(), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Gear Level");
            ImGui::TableSetupColumn("Forward/Backward Speed");
            ImGui::TableSetupColumn("Left/Right Speed");
            ImGui::TableHeadersRow();
            for (int level = 1; level <= kMaxPlayerGearLevel; ++level) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", level);
                OriGine::Vec2f directionalSpeed = CalculateCurrentMaxDirectionalSpeed(level);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", directionalSpeed[X]);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.2f", directionalSpeed[Y]);
            }
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }

    ImGui::Spacing();

    DragGuiVectorCommand("Min Smooth Time##" + _parentLabel, minSmoothTime_, 0.01f);
    DragGuiVectorCommand("Max Smooth Time##" + _parentLabel, maxSmoothTime_, 0.01f);

    ImGui::Spacing();

    DragGuiVectorCommand("Min Limit Directional Accel##" + _parentLabel, minLimitDirectionalAccel_, 0.01f);
    DragGuiVectorCommand("Max Limit Directional Accel##" + _parentLabel, maxLimitDirectionalAccel_, 0.01f);

    ImGui::Spacing();

    label = "CoolTime##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        DragGuiCommand("gearUpCoolTime##" + _parentLabel, baseGearupCoolTime_, 0.01f);
        DragGuiCommand("coolTimeAddRateBase##" + _parentLabel, coolTimeAddRateBase_, 0.01f);
        DragGuiCommand("coolTimeAddRateCommonRate##" + _parentLabel, coolTimeAddRateCommonRate_, 0.01f);

        label = "CoolTimeByGearLevel##" + _parentLabel;
        if (ImGui::BeginTable("CoolTimeByGearLevel", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Gear Level");
            ImGui::TableSetupColumn("CoolTime");
            ImGui::TableHeadersRow();
            for (int level = 1; level <= kMaxPlayerGearLevel; ++level) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", level);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", CalculateCoolTimeByGearLevel(level));
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    ImGui::Spacing();

    DragGuiCommand("Invincibility Time##" + _parentLabel, invincibilityTime_, 0.01f);
    DragGuiCommand("Wall Run Interval##" + _parentLabel, wallRunInterval_, 0.01f);

    ImGui::Spacing();

    DragGuiCommand("Direction Interpolate Rate##" + _parentLabel, directionInterpolateRate_, 0.01f);

    ImGui::Spacing();

    if (ImGui::TreeNode("JumpHoldVelocity")) {
        EasingComboGui("EaseType##JumpHoldVelocity" + _parentLabel, jumpHoldVelocityEaseType_);

        DragGuiCommand("Min##JumpHoldVelocity##" + _parentLabel, minJumpHoldVelocity_, 0.01f);
        DragGuiCommand("Max##JumpHoldVelocity##" + _parentLabel, maxJumpHoldVelocity_, 0.01f);

        ImGui::TreePop();
    }

    ImGui::Spacing();

    if (ImGui::TreeNode("JumpChargeRate")) {
        EasingComboGui("EaseType##JumpChargeRate##" + _parentLabel, jumpChargeRateEaseType_);

        DragGuiCommand("Min##JumpChargeRate##" + _parentLabel, minJumpChargeRate_, 0.01f);
        DragGuiCommand("Max##JumpChargeRate##" + _parentLabel, maxJumpChargeRate_, 0.01f);

        ImGui::TreePop();
    }

    ImGui::Spacing();

    DragGuiCommand("WallRunRate##" + _parentLabel, wallRunRate_, 0.01f);
    DragGuiCommand("WallRunRampUpTime##" + _parentLabel, wallRunRampUpTime_, 0.01f);

    DragGuiCommand("MinWallJumpOffsetX##" + _parentLabel, minWallJumpOffsetX_, 0.01f, 0.f, wallJumpOffset_[X]);
    DragGuiVectorCommand<3, float>("WallJumpOffset##" + _parentLabel, wallJumpOffset_, 0.01f);
    wallJumpOffset_ = wallJumpOffset_.normalize();

    DragGuiCommand("GravityApplyDelay On WallRun##" + _parentLabel, gravityApplyDelayOnWallRun_, 0.01f);
    DragGuiCommand("WallRunDetachSpeed##" + _parentLabel, wallRunDetachSpeed_, 0.01f);

    ImGui::Spacing();

    DragGuiCommand("RailSpeedRate##" + _parentLabel, railSpeedRate_, 0.01f);
    DragGuiCommand("RailRampUpTime##" + _parentLabel, railRampUpTime_, 0.01f);
    DragGuiVectorCommand<3, float>("RailJumpOffset##" + _parentLabel, railJumpOffset_, 0.01f);
    railJumpOffset_ = railJumpOffset_.normalize();
    DragGuiCommand("RailInterval##" + _parentLabel, railInterval_, 0.01f);

    ImGui::Spacing();
    DragGuiCommand("Default Mass##" + _parentLabel, defaultMass_, 0.01f);
    DragGuiCommand("Mass On Wall Run##" + _parentLabel, massOnWallRun_, 0.01f);

    ImGui::Spacing();

    DragGuiCommand("Ground Check Threshold##" + _parentLabel, groundCheckThreshold_, 0.01f);
    DragGuiCommand("Wall Check Threshold##" + _parentLabel, wallCheckThreshold_, 0.01f);

#endif // _DEBUG
}

void PlayerStatus::Debug(Scene* /*_scene*/, EntityHandle /*_handle*/, const std::string& /*_parentLabel*/) {
#ifdef _DEBUG

    ImGui::Text("Base Gear Up Cool Time : %.2f", baseGearupCoolTime_);
    ImGui::Text("Gear Up Cool Time      : %.2f", gearUpCoolTime_);
    ImGui::Spacing();
    ImGui::Text("Base Speed             : %.2f", baseSpeed_);
    ImGui::Text("CurrentMax Speed       : %.2f", currentMaxSpeed_);

    if (ImGui::TreeNode("JumpHoldVelocity")) {
        ImGui::Text("EaseType : %s", EasingNames[jumpHoldVelocityEaseType_].c_str());

        ImGui::Text("Min : %.3", minJumpHoldVelocity_);
        ImGui::Text("Max : %.3", maxJumpHoldVelocity_);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("JumpChargeRate")) {
        ImGui::Text("EaseType : %s", EasingNames[jumpHoldVelocityEaseType_].c_str());

        ImGui::Text("Min : %.3", minJumpChargeRate_);
        ImGui::Text("Max : %.3", maxJumpChargeRate_);

        ImGui::TreePop();
    }

    ImGui::Spacing();
    ImGui::Text("Speed Up Rate Base        : %.2f", speedUpRateBase_);
    ImGui::Text("Speed Up Rate Common Rate : %.2f", speedUpRateCommonRate_);
    ImGui::Spacing();
    ImGui::Text("Cool Time Up Rate Base        : %.2f", coolTimeAddRateBase_);
    ImGui::Text("Cool Time Up Rate Common Rate : %.2f", coolTimeAddRateCommonRate_);
    ImGui::Spacing();
    ImGui::Text("Wall Run Rate             : %.2f", wallRunRate_);
    ImGui::Text("Wall Run Ramp Up Time     : %.2f", wallRunRampUpTime_);
    ImGui::Text("Wall Jump Offset          : (%.2f, %.2f, %.2f)", wallJumpOffset_[X], wallJumpOffset_[Y], wallJumpOffset_[Z]);
    ImGui::Text("Direction Interpolate Rate: %.2f", directionInterpolateRate_);
#endif
}

void PlayerStatus::Finalize() {}

float PlayerStatus::CalculateSpeedByGearLevel(int32_t _gearLevel) const {
    return ArithmeticSequence<float>(
        baseSpeed_,
        ArithmeticSequence<float>(speedUpRateBase_, speedUpRateCommonRate_, _gearLevel - 1),
        _gearLevel);
}

float PlayerStatus::CalculateCoolTimeByGearLevel(int32_t _gearLevel) const {
    return GeometricSequence<float>(
        baseGearupCoolTime_,
        ArithmeticSequence<float>(coolTimeAddRateBase_, coolTimeAddRateCommonRate_, _gearLevel - 1),
        _gearLevel);
}

OriGine::Vec2f PlayerStatus::CalculateCurrentMaxDirectionalSpeed(int32_t _gearLevel) const {
    float forwardBackwardSpeed = ArithmeticSequence<float>(
        baseDirectionalSpeed_[X],
        ArithmeticSequence<float>(directionalSpeedUpRateBase_[X], directionalSpeedUpRateCommonRate_[X], _gearLevel - 1),
        _gearLevel);
    float leftRightSpeed = ArithmeticSequence<float>(
        baseDirectionalSpeed_[Y],
        ArithmeticSequence<float>(directionalSpeedUpRateBase_[Y], directionalSpeedUpRateCommonRate_[Y], _gearLevel - 1),
        _gearLevel);
    return {forwardBackwardSpeed, leftRightSpeed};
}

void PlayerStatus::SetupOnGearUp(int32_t _gearLevel) {
    currentMaxSpeed_            = CalculateSpeedByGearLevel(_gearLevel);
    gearUpCoolTime_             = CalculateCoolTimeByGearLevel(_gearLevel);
    currentMaxDirectionalSpeed_ = CalculateCurrentMaxDirectionalSpeed(_gearLevel);
}

Vec3f PlayerStatus::ComputeSmoothedDirection(const Vec3f& _targetDir, const Rigidbody* _rigidbody, const Transform* _transform, float _deltaTime) const {
    constexpr float kMaxRate = 2.34f; // 逆向きのときの最大補間速度倍率

    // 入力がない場合は現在の向きを維持
    if (_targetDir.lengthSq() <= kEpsilon) {
        Vec3f currentDir = _rigidbody->GetVelocity();
        currentDir[Y]    = 0.0f;
        if (currentDir.lengthSq() <= kEpsilon) {
            currentDir = axisZ * MakeMatrix4x4::RotateQuaternion(_transform->rotate);
        }
        return currentDir.normalize();
    }

    // 現在のXZ平面の速度を取得
    Vec3f currentDir = _rigidbody->GetVelocity();
    currentDir[Y]    = 0.0f;

    // 停止している場合は、現在の体の向きを基準にする
    if (currentDir.lengthSq() <= kEpsilon) {
        currentDir = axisZ * MakeMatrix4x4::RotateQuaternion(_transform->rotate);
    }
    currentDir = currentDir.normalize();

    // 現在の方向と目標方向の内積に応じて補間速度を調整
    float dot  = Vec3f::Dot(currentDir, _targetDir);
    float rate = directionInterpolateRate_;
    if (dot < 0.0f) {
        // 逆向きほど補間速度を上げる (dot=-1 で最大倍率)
        rate *= std::lerp(1.0f, kMaxRate, -dot);
    }

    // 補間計算
    Vec3f resultDir = LerpByDeltaTime(currentDir, _targetDir, _deltaTime, rate);
    return resultDir.normalize();
}

void to_json(nlohmann::json& _j, const PlayerStatus& _playerStatus) {
    _j["decelerationFactor"]    = _playerStatus.decelerationFactor_;
    _j["baseSpeed"]             = _playerStatus.baseSpeed_;
    _j["speedUpRateBase"]       = _playerStatus.speedUpRateBase_;
    _j["speedUpRateCommonRate"] = _playerStatus.speedUpRateCommonRate_;

    _j["baseDirectionalSpeed"]             = _playerStatus.baseDirectionalSpeed_;
    _j["directionalSpeedUpRateBase"]       = _playerStatus.directionalSpeedUpRateBase_;
    _j["directionalSpeedUpRateCommonRate"] = _playerStatus.directionalSpeedUpRateCommonRate_;

    _j["minSmoothTime"]            = _playerStatus.minSmoothTime_;
    _j["maxSmoothTime"]            = _playerStatus.maxSmoothTime_;
    _j["minLimitDirectionalAccel"] = _playerStatus.minLimitDirectionalAccel_;
    _j["maxLimitDirectionalAccel"] = _playerStatus.maxLimitDirectionalAccel_;

    _j["coolTimeAddRateBase"]       = _playerStatus.coolTimeAddRateBase_;
    _j["coolTimeAddRateCommonRate"] = _playerStatus.coolTimeAddRateCommonRate_;

    _j["wallRunRate"]        = _playerStatus.wallRunRate_;
    _j["wallRunRampUpTime"]  = _playerStatus.wallRunRampUpTime_;
    _j["minWallJumpOffsetX"] = _playerStatus.minWallJumpOffsetX_;
    _j["wallJumpOffset"]     = _playerStatus.wallJumpOffset_;

    _j["jumpHoldVelocityEaseType"] = static_cast<int>(_playerStatus.jumpHoldVelocityEaseType_);
    _j["minJumpHoldVelocity"]      = _playerStatus.minJumpHoldVelocity_;
    _j["maxJumpHoldVelocity"]      = _playerStatus.maxJumpHoldVelocity_;
    _j["jumpChargeRateEaseType"]   = static_cast<int>(_playerStatus.jumpChargeRateEaseType_);
    _j["minJumpChargeRate"]        = _playerStatus.minJumpChargeRate_;
    _j["maxJumpChargeRate"]        = _playerStatus.maxJumpChargeRate_;

    _j["gearUpCoolTime"]           = _playerStatus.baseGearupCoolTime_;
    _j["directionInterpolateRate"] = _playerStatus.directionInterpolateRate_;

    _j["gravityApplyDelayOnWallRun"] = _playerStatus.gravityApplyDelayOnWallRun_;
    _j["wallRunDetachSpeed"]         = _playerStatus.wallRunDetachSpeed_;

    _j["defaultMass"]   = _playerStatus.defaultMass_;
    _j["massOnWallRun"] = _playerStatus.massOnWallRun_;

    _j["wallRunInterval"] = _playerStatus.wallRunInterval_;

    _j["railSpeedRate"]  = _playerStatus.railSpeedRate_;
    _j["railRampUpTime"] = _playerStatus.railRampUpTime_;
    _j["railJumpOffset"] = _playerStatus.railJumpOffset_;
    _j["railInterval"]   = _playerStatus.railInterval_;

    _j["groundCheckThreshold"] = _playerStatus.groundCheckThreshold_;
    _j["wallCheckThreshold"]   = _playerStatus.wallCheckThreshold_;
}
void from_json(const nlohmann::json& _j, PlayerStatus& _playerStatus) {
    if (_j.contains("decelerationFactor")) {
        _j.at("decelerationFactor").get_to(_playerStatus.decelerationFactor_);
    }

    _j.at("baseSpeed").get_to(_playerStatus.baseSpeed_);
    _j.at("speedUpRateBase").get_to(_playerStatus.speedUpRateBase_);
    _j.at("speedUpRateCommonRate").get_to(_playerStatus.speedUpRateCommonRate_);

    if (_j.contains("baseDirectionalSpeed")) {
        _j.at("baseDirectionalSpeed").get_to(_playerStatus.baseDirectionalSpeed_);
    }
    if (_j.contains("directionalSpeedUpRateBase")) {
        _j.at("directionalSpeedUpRateBase").get_to(_playerStatus.directionalSpeedUpRateBase_);
    }
    if (_j.contains("directionalSpeedUpRateCommonRate")) {
        _j.at("directionalSpeedUpRateCommonRate").get_to(_playerStatus.directionalSpeedUpRateCommonRate_);
    }

    if (_j.contains("minSmoothTime")) {
        _j.at("minSmoothTime").get_to(_playerStatus.minSmoothTime_);
    }
    if (_j.contains("maxSmoothTime")) {
        _j.at("maxSmoothTime").get_to(_playerStatus.maxSmoothTime_);
    }

    if (_j.contains("minLimitDirectionalAccel")) {
        _j.at("minLimitDirectionalAccel").get_to(_playerStatus.minLimitDirectionalAccel_);
    }
    if (_j.contains("maxLimitDirectionalAccel")) {
        _j.at("maxLimitDirectionalAccel").get_to(_playerStatus.maxLimitDirectionalAccel_);
    }

    _j.at("coolTimeAddRateBase").get_to(_playerStatus.coolTimeAddRateBase_);
    _j.at("coolTimeAddRateCommonRate").get_to(_playerStatus.coolTimeAddRateCommonRate_);

    int easetype = 0;
    _j.at("jumpHoldVelocityEaseType").get_to(easetype);
    _playerStatus.jumpHoldVelocityEaseType_ = static_cast<EaseType>(easetype);
    _j.at("minJumpHoldVelocity").get_to(_playerStatus.minJumpHoldVelocity_);
    _j.at("maxJumpHoldVelocity").get_to(_playerStatus.maxJumpHoldVelocity_);

    _j.at("jumpChargeRateEaseType").get_to(easetype);
    _playerStatus.jumpChargeRateEaseType_ = static_cast<EaseType>(easetype);
    _j.at("minJumpChargeRate").get_to(_playerStatus.minJumpChargeRate_);
    _j.at("maxJumpChargeRate").get_to(_playerStatus.maxJumpChargeRate_);

    _j.at("wallRunRate").get_to(_playerStatus.wallRunRate_);
    _j.at("wallRunRampUpTime").get_to(_playerStatus.wallRunRampUpTime_);
    _j.at("wallJumpOffset").get_to(_playerStatus.wallJumpOffset_);
    if (_j.contains("minWallJumpOffsetX")) {
        _j.at("minWallJumpOffsetX").get_to(_playerStatus.minWallJumpOffsetX_);
    }
    _j.at("gearUpCoolTime").get_to(_playerStatus.baseGearupCoolTime_);

    _j.at("directionInterpolateRate").get_to(_playerStatus.directionInterpolateRate_);

    if (_j.contains("railRampUpTime")) {
        _j.at("railRampUpTime").get_to(_playerStatus.railRampUpTime_);
    }
    if (_j.contains("railSpeedRate")) {
        _j.at("railSpeedRate").get_to(_playerStatus.railSpeedRate_);
    }
    if (_j.contains("railJumpOffset")) {
        _j.at("railJumpOffset").get_to(_playerStatus.railJumpOffset_);
    }

    if (_j.contains("gravityApplyDelayOnWallRun")) {
        _j.at("gravityApplyDelayOnWallRun").get_to(_playerStatus.gravityApplyDelayOnWallRun_);
    }
    if (_j.contains("wallRunDetachSpeed")) {
        _j.at("wallRunDetachSpeed").get_to(_playerStatus.wallRunDetachSpeed_);
    }

    if (_j.contains("defaultMass")) {
        _j.at("defaultMass").get_to(_playerStatus.defaultMass_);
    }
    if (_j.contains("massOnWallRun")) {
        _j.at("massOnWallRun").get_to(_playerStatus.massOnWallRun_);
    }

    if (_j.contains("wallRunInterval")) {
        _j.at("wallRunInterval").get_to(_playerStatus.wallRunInterval_);
    }
    if (_j.contains("railInterval")) {
        _j.at("railInterval").get_to(_playerStatus.railInterval_);
    }

    if (_j.contains("groundCheckThreshold")) {
        _j.at("groundCheckThreshold").get_to(_playerStatus.groundCheckThreshold_);
    }
    if (_j.contains("wallCheckThreshold")) {
        _j.at("wallCheckThreshold").get_to(_playerStatus.wallCheckThreshold_);
    }
}
