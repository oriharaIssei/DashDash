#include "PathController.h"

#ifdef _DEBUG
/// editor
#include "editor/EditorController.h"

/// util
#include "util/myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

void PathController::Initialize(OriGine::Scene* /*_scene*/, const OriGine::EntityHandle& /*_owner*/) {
    currentIndex_ = startIndex_;
    progress_     = startProgress_;
}
void PathController::Finalize() {}

void PathController::Edit(OriGine::Scene* /*_scene*/, const OriGine::EntityHandle& /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    CheckBoxCommand("Is Active##" + _parentLabel, isActive_);
    CheckBoxCommand("Is Playing##" + _parentLabel, isPlaying_);
    DragGuiCommand("Speed##" + _parentLabel, speed_, 0.01f, 0.f);
    DragGuiCommand<int32_t>("Start Index##" + _parentLabel, startIndex_, 1, 0, static_cast<int32_t>(points_.size()) - 2, "%d");
    DragGuiCommand("Start Progress##" + _parentLabel, startProgress_, 0.01f, 0.0f, 1.0f);

    ImGui::Spacing();

    // PlayOptions combo
    {
        const char* playOptionItems[] = {"Once", "Loop", "PingPong"};
        int playOptionInt             = static_cast<int>(playOption_);
        if (ImGui::Combo(("Play Option##" + _parentLabel).c_str(), &playOptionInt, playOptionItems, 3)) {
            playOption_ = static_cast<PlayOptions>(playOptionInt);
        }
    }

    // InterpolationType combo
    {
        const char* interpolationItems[] = {"Linear", "Bezier", "CatmullRom"};
        int interpolationInt             = static_cast<int>(interpolation_);
        if (ImGui::Combo(("Interpolation##" + _parentLabel).c_str(), &interpolationInt, interpolationItems, 3)) {
            interpolation_ = static_cast<InterpolationType>(interpolationInt);
        }
    }

    ImGui::Spacing();

    // RotationMode combo
    {
        const char* rotationModeItems[] = {"Fixed", "FaceForward", "FaceForwardSmooth"};
        int rotationModeInt             = static_cast<int>(rotationMode_);
        if (ImGui::Combo(("Rotation Mode##" + _parentLabel).c_str(), &rotationModeInt, rotationModeItems, 3)) {
            rotationMode_ = static_cast<RotationMode>(rotationModeInt);
        }
    }

    if (rotationMode_ == RotationMode::FaceForwardSmooth) {
        DragGuiCommand("Rotation Smooth Speed##" + _parentLabel, rotationSmoothSpeed_, 0.1f, 0.f);
    }

    ImGui::Spacing();
    ImGui::Separator();

    // ポイント追加ボタン
    if (ImGui::Button(("Add Point##" + _parentLabel).c_str())) {
        Vector3f newPoint = Vector3f{};
        if (!points_.empty()) {
            newPoint = points_.back() + Vector3f{0.f, 0.f, 1.f};
        }
        auto command = std::make_unique<AddElementCommand<std::vector<Vector3f>>>(&points_, newPoint);
        EditorController::GetInstance()->PushCommand(std::move(command));
    }

    ImGui::Separator();

    // ポイントリスト表示
    for (int i = 0; i < static_cast<int>(points_.size()); ++i) {
        ImGui::PushID(i);

        // 削除ボタン
        if (ImGui::Button(("X##Point" + std::to_string(i) + _parentLabel).c_str())) {
            auto command = std::make_unique<EraseElementCommand<std::vector<Vector3f>>>(&points_, points_.begin() + i);
            EditorController::GetInstance()->PushCommand(std::move(command));
            ImGui::PopID();
            break;
        }
        ImGui::SameLine();

        // ポイント位置編集
        DragGuiVectorCommand<3, float>("Point " + std::to_string(i) + "##" + _parentLabel, points_[i], 0.1f);

        ImGui::PopID();
    }

    ImGui::Separator();

#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const PathController& _c) {
    _j = nlohmann::json{
        {"isActive", _c.isActive_},
        {"isReversed", _c.isReversed_},
        {"isPlaying", _c.isPlaying_},
        {"speed", _c.speed_},
        {"startIndex", _c.startIndex_},
        {"startProgress", _c.startProgress_},
        {"currentIndex", _c.currentIndex_},
        {"progress", _c.progress_},
        {"playOption", static_cast<int>(_c.playOption_)},
        {"interpolation", static_cast<int>(_c.interpolation_)},
        {"rotationMode", static_cast<int>(_c.rotationMode_)},
        {"rotationSmoothSpeed", _c.rotationSmoothSpeed_},
        {"points", _c.points_},
    };
}

void from_json(const nlohmann::json& _j, PathController& _c) {
    _j.at("isActive").get_to(_c.isActive_);
    _j.at("isReversed").get_to(_c.isReversed_);
    _j.at("isPlaying").get_to(_c.isPlaying_);
    _j.at("speed").get_to(_c.speed_);
    _c.startIndex_    = _j.value("startIndex", 0);
    _c.startProgress_ = _j.value("startProgress", 0.0f);
    _j.at("currentIndex").get_to(_c.currentIndex_);
    _j.at("progress").get_to(_c.progress_);
    _c.playOption_          = static_cast<PathController::PlayOptions>(_j.value("playOption", 0));
    _c.interpolation_       = static_cast<PathController::InterpolationType>(_j.value("interpolation", 0));
    _c.rotationMode_        = static_cast<PathController::RotationMode>(_j.value("rotationMode", 0));
    _c.rotationSmoothSpeed_ = _j.value("rotationSmoothSpeed", 5.0f);
    _j.at("points").get_to(_c.points_);
}
