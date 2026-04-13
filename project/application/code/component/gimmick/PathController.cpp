#include "PathController.h"

#ifdef _DEBUG
/// editor
#include "editor/EditorController.h"

/// util
#include "util/myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

void PathController::Initialize(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/) {
    currentIndex = startIndex;
    progress     = startProgress;
}
void PathController::Finalize() {}

void PathController::Edit(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    CheckBoxCommand("Is Active##" + _parentLabel, isActive);
    CheckBoxCommand("Is Playing##" + _parentLabel, isPlaying);
    DragGuiCommand("Speed##" + _parentLabel, speed, 0.01f, 0.f);
    DragGuiCommand<int32_t>("Start Index##" + _parentLabel, startIndex, 1, 0, static_cast<int32_t>(points.size()) - 2, "%d");
    DragGuiCommand("Start Progress##" + _parentLabel, startProgress, 0.01f, 0.0f, 1.0f);

    ImGui::Spacing();

    // PlayOptions combo
    {
        const char* playOptionItems[] = {"Once", "Loop", "PingPong"};
        int playOptionInt             = static_cast<int>(playOption);
        if (ImGui::Combo(("Play Option##" + _parentLabel).c_str(), &playOptionInt, playOptionItems, 3)) {
            playOption = static_cast<PlayOptions>(playOptionInt);
        }
    }

    // InterpolationType combo
    {
        const char* interpolationItems[] = {"Linear", "Bezier", "CatmullRom"};
        int interpolationInt             = static_cast<int>(interpolation);
        if (ImGui::Combo(("Interpolation##" + _parentLabel).c_str(), &interpolationInt, interpolationItems, 3)) {
            interpolation = static_cast<InterpolationType>(interpolationInt);
        }
    }

    ImGui::Spacing();

    // RotationMode combo
    {
        const char* rotationModeItems[] = {"Fixed", "FaceForward", "FaceForwardSmooth"};
        int rotationModeInt             = static_cast<int>(rotationMode);
        if (ImGui::Combo(("Rotation Mode##" + _parentLabel).c_str(), &rotationModeInt, rotationModeItems, 3)) {
            rotationMode = static_cast<RotationMode>(rotationModeInt);
        }
    }

    if (rotationMode == RotationMode::FaceForwardSmooth) {
        DragGuiCommand("Rotation Smooth Speed##" + _parentLabel, rotationSmoothSpeed, 0.1f, 0.f);
    }

    ImGui::Spacing();
    ImGui::Separator();

    // ポイント追加ボタン
    if (ImGui::Button(("Add Point##" + _parentLabel).c_str())) {
        Vector3f newPoint = Vector3f{};
        if (!points.empty()) {
            newPoint = points.back() + Vector3f{0.f, 0.f, 1.f};
        }
        auto command = std::make_unique<AddElementCommand<std::vector<Vector3f>>>(&points, newPoint);
        EditorController::GetInstance()->PushCommand(std::move(command));
    }

    ImGui::Separator();

    // ポイントリスト表示
    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        ImGui::PushID(i);

        // 削除ボタン
        if (ImGui::Button(("X##Point" + std::to_string(i) + _parentLabel).c_str())) {
            auto command = std::make_unique<EraseElementCommand<std::vector<Vector3f>>>(&points, points.begin() + i);
            EditorController::GetInstance()->PushCommand(std::move(command));
            ImGui::PopID();
            break;
        }
        ImGui::SameLine();

        // ポイント位置編集
        DragGuiVectorCommand<3, float>("Point " + std::to_string(i) + "##" + _parentLabel, points[i], 0.1f);

        ImGui::PopID();
    }

    ImGui::Separator();

#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const PathController& _c) {
    _j = nlohmann::json{
        {"isActive", _c.isActive},
        {"isReversed", _c.isReversed},
        {"isPlaying", _c.isPlaying},
        {"speed", _c.speed},
        {"startIndex", _c.startIndex},
        {"startProgress", _c.startProgress},
        {"currentIndex", _c.currentIndex},
        {"progress", _c.progress},
        {"playOption", static_cast<int>(_c.playOption)},
        {"interpolation", static_cast<int>(_c.interpolation)},
        {"rotationMode", static_cast<int>(_c.rotationMode)},
        {"rotationSmoothSpeed", _c.rotationSmoothSpeed},
        {"points", _c.points},
    };
}

void from_json(const nlohmann::json& _j, PathController& _c) {
    _j.at("isActive").get_to(_c.isActive);
    _j.at("isReversed").get_to(_c.isReversed);
    _j.at("isPlaying").get_to(_c.isPlaying);
    _j.at("speed").get_to(_c.speed);
    _c.startIndex    = _j.value("startIndex", 0);
    _c.startProgress = _j.value("startProgress", 0.0f);
    _j.at("currentIndex").get_to(_c.currentIndex);
    _j.at("progress").get_to(_c.progress);
    _c.playOption          = static_cast<PathController::PlayOptions>(_j.value("playOption", 0));
    _c.interpolation       = static_cast<PathController::InterpolationType>(_j.value("interpolation", 0));
    _c.rotationMode        = static_cast<PathController::RotationMode>(_j.value("rotationMode", 0));
    _c.rotationSmoothSpeed = _j.value("rotationSmoothSpeed", 5.0f);
    _j.at("points").get_to(_c.points);
}
