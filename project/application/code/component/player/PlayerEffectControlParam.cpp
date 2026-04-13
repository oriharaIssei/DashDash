#include "PlayerEffectControlParam.h"

#ifdef _DEBUG
#include <myGui/MyGui.h>
#endif // _DEBUG

using namespace OriGine;

PlayerEffectControlParam::PlayerEffectControlParam() {}
PlayerEffectControlParam::~PlayerEffectControlParam() {}

void PlayerEffectControlParam::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    if (trailColorByGearLevel_.empty()) {
        trailColorByGearLevel_ = std::vector<OriGine::Vec4f>(kMaxPlayerGearLevel, OriGine::Vec4f(1.f, 1.f, 1.f, 1.f));
    }
}
void PlayerEffectControlParam::Finalize() {}

float PlayerEffectControlParam::CalculateWheelSpinSpeedBySpeed(float _currentSpeed, float _maxSpeed) const {
    return std::lerp(0.0f, maxWheelSpinSpeed_, _currentSpeed / _maxSpeed);
}

float PlayerEffectControlParam::CalculateWheelTiltAngle(const OriGine::Vec3f& _inputV, const OriGine::Vec3f& _direction) const {
    // 進行方向のXZ平面への射影を正規化
    OriGine::Vec3f forwardDir = _direction;
    forwardDir[Y]             = 0.0f;
    forwardDir                = Vector3f::Normalize(forwardDir);
    // 入力ベクトルを3Dベクトルに変換（Zが前、Xが右）
    OriGine::Vec3f inputDir3D = {_inputV[X], 0.0f, _inputV[Z]};
    inputDir3D                = Vector3f::Normalize(inputDir3D);
    // 進行方向と入力方向の角度差を計算
    float dotProduct      = Vector3f::Dot(forwardDir, inputDir3D);
    dotProduct            = std::clamp(dotProduct, -1.0f, 1.0f); // 安全のためクランプ
    float angleDifference = std::acos(dotProduct); // ラジアン
    // 外積を使って回転方向を決定
    OriGine::Vec3f crossProduct = forwardDir.cross(inputDir3D);
    if (crossProduct[Y] > 0) {
        angleDifference = -angleDifference; // 左回りの場合は負の角度にする
    }
    // ホイールの傾き角度を計算
    float result = angleDifference * wheelTiltAngleRate_;
    return result;
}

void PlayerEffectControlParam::Edit(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_handle*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    // ギアレベルに応じたトレイルカラー
    std::string label = "";

    if (ImGui::BeginTable("TrailColorByGearLevel", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Gear Level");
        ImGui::TableSetupColumn("TrailColor");
        ImGui::TableHeadersRow();
        for (int level = 1; level <= kMaxPlayerGearLevel; ++level) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", level);
            ImGui::TableSetColumnIndex(1);
            label = "TrailColor##" + std::to_string(level) + _parentLabel;
            ColorEditGuiCommand(label, trailColorByGearLevel_[level - 1]);
        }
        ImGui::EndTable();
    }

    // 壁走り時の回転オフセット
    DragGuiCommand("rotateOffsetOnWallRun##" + _parentLabel, rotateOffsetOnWallRun_, 0.01f);
    // 移動時の回転速度率
    DragGuiCommand("maxWheelSpinSpeed##" + _parentLabel, maxWheelSpinSpeed_, 0.001f);
    // 移動時のホイール傾き角度
    DragGuiCommand("wheelTiltAngleRate##" + _parentLabel, wheelTiltAngleRate_, 0.001f);
    // ホイール傾き角度の最大加速度
    DragGuiCommand("wheelTiltAngleMaxAccel##" + _parentLabel, wheelTiltAngleMaxAccel_, 0.001f);
    // ホイール傾き角度の最大加速度
    DragGuiCommand("maxWheelTiltAngle##" + _parentLabel, maxWheelTiltAngle_, 0.001f);

    ImGui::Spacing();

    // レール走行時の最大傾き角度
    DragGuiCommand("max tilt on rail run##" + _parentLabel, maxTiltOnRailRun_, 0.01f);
    DragGuiCommand("tilt speed on rail run##" + _parentLabel, tiltSpeedOnRailRun_, 0.01f);

    ImGui::Spacing();

    // 無敵時点滅エフェクト
    DragGuiCommand("invincibleBlinkBaseAmplitude##" + _parentLabel, invincibleBlinkBaseAmplitude_, 0.1f);
    DragGuiCommand("invincibleBlinkMaxAmplitude##" + _parentLabel, invincibleBlinkMaxAmplitude_, 0.1f);

    ImGui::Spacing();

    DragGuiCommand("aheadCollisionTiltAngle##" + _parentLabel, aheadCollisionTiltAngle_, 0.001f, {}, {}, "%.4f");
    DragGuiCommand("aheadCollisionTiltSpeed##" + _parentLabel, aheadCollisionTiltSpeed_, 0.001f, {}, {}, "%.4f");

    ImGui::Spacing();

    DragGuiCommand("thresholdSpeedlineParticle##" + _parentLabel, thresholdSpeedlineParticle_, 0.1f);

#endif // _DEBUG
}

void to_json(nlohmann::json& j, const PlayerEffectControlParam& _p) {
    j = nlohmann::json{
        {"trailColorByGearLevel", _p.trailColorByGearLevel_},
        {"rotateOffsetOnWallRun", _p.rotateOffsetOnWallRun_},
        {"maxWheelSpinSpeed", _p.maxWheelSpinSpeed_},
        {"wheelTiltAngleRate", _p.wheelTiltAngleRate_},
        {"maxWheelTiltAngle", _p.maxWheelTiltAngle_},
        {"wheelTiltAngleMaxAccel", _p.wheelTiltAngleMaxAccel_},
        {"maxTiltOnRailRun", _p.maxTiltOnRailRun_},
        {"tiltSpeedOnRailRun", _p.tiltSpeedOnRailRun_},
        {"invincibleBlinkBaseAmplitude", _p.invincibleBlinkBaseAmplitude_},
        {"invincibleBlinkMaxAmplitude", _p.invincibleBlinkMaxAmplitude_},
        {"aheadCollisionTiltAngle", _p.aheadCollisionTiltAngle_},
        {"aheadCollisionTiltSpeed", _p.aheadCollisionTiltSpeed_},
        {"thresholdSpeedlineParticle", _p.thresholdSpeedlineParticle_},
    };
}

void from_json(const nlohmann::json& j, PlayerEffectControlParam& _p) {
    j.at("trailColorByGearLevel").get_to(_p.trailColorByGearLevel_);

    while (_p.trailColorByGearLevel_.size() <= kMaxPlayerGearLevel) {
        _p.trailColorByGearLevel_.emplace_back(OriGine::Vec4f(1.f, 1.f, 1.f, 1.f));
    }

    while (_p.trailColorByGearLevel_.size() > kMaxPlayerGearLevel) {
        _p.trailColorByGearLevel_.pop_back();
    }

    j.at("rotateOffsetOnWallRun").get_to(_p.rotateOffsetOnWallRun_);
    j.at("maxWheelSpinSpeed").get_to(_p.maxWheelSpinSpeed_);
    j.at("wheelTiltAngleRate").get_to(_p.wheelTiltAngleRate_);
    j.at("wheelTiltAngleMaxAccel").get_to(_p.wheelTiltAngleMaxAccel_);

    if (j.contains("maxWheelTiltAngle")) {
        j.at("maxWheelTiltAngle").get_to(_p.maxWheelTiltAngle_);
    }

    if (j.contains("maxTiltOnRailRun")) {
        j.at("maxTiltOnRailRun").get_to(_p.maxTiltOnRailRun_);
    }
    if (j.contains("tiltSpeedOnRailRun")) {
        j.at("tiltSpeedOnRailRun").get_to(_p.tiltSpeedOnRailRun_);
    }

    if (j.contains("invincibleBlinkBaseAmplitude")) {
        j.at("invincibleBlinkBaseAmplitude").get_to(_p.invincibleBlinkBaseAmplitude_);
    }
    if (j.contains("invincibleBlinkMaxAmplitude")) {
        j.at("invincibleBlinkMaxAmplitude").get_to(_p.invincibleBlinkMaxAmplitude_);
    }
    if (j.contains("aheadCollisionTiltAngle")) {
        j.at("aheadCollisionTiltAngle").get_to(_p.aheadCollisionTiltAngle_);
    }
    if (j.contains("aheadCollisionTiltSpeed")) {
        j.at("aheadCollisionTiltSpeed").get_to(_p.aheadCollisionTiltSpeed_);
    }
    if (j.contains("thresholdSpeedlineParticle")) {
        j.at("thresholdSpeedlineParticle").get_to(_p.thresholdSpeedlineParticle_);
    }
}
