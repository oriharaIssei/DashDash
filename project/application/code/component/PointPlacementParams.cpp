#include "PointPlacementParams.h"

/// Engine
#include "asset/AssetSystem.h"
// asset
#include "asset/TextureAsset.h"

// directX12
#include "directX12/DxDevice.h"

#ifdef _DEBUG
/// Engine
#define RESOURCE_DIRECTORY
#include "EngineInclude.h"

/// util
#include "myFileSystem/MyFileSystem.h"
#include "myGui/MyGui.h"

#endif // _DEBUG

using namespace OriGine;

void PointPlacementParams::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    auto& device = Engine::GetInstance()->GetDxDevice()->device_;
    paramData_.CreateBuffer(device);
    paramData_.ConvertToBuffer();

    size_t outputCapacity = static_cast<size_t>(paramData_.openData_.placementResolution * paramData_.openData_.placementResolution);
    outputPoints_.CreateBuffer(device, outputCapacity);

    // テクスチャの読み込み
    if (!pointDensityTexturePath_.empty()) {
        pointDensityTextureIndex_ = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(pointDensityTexturePath_);
    }
    if (!pointVolumeTexturePath_.empty()) {
        pointVolumeTextureIndex_ = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(pointVolumeTexturePath_);
    }
    if (!pointMaskTexturePath_.empty()) {
        pointMaskTextureIndex_ = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(pointMaskTexturePath_);
    }
}

void PointPlacementParams::Finalize() {
    paramData_.Finalize();
    outputPoints_.Finalize();
}

void PointPlacementParams::Edit(Scene* /*_scene*/, EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    ImGui::Text("OriginalEntity \n Directory : %s \n FileName  : %s", originalEntityFilePath_.first.c_str(), originalEntityFilePath_.second.c_str());

    std::string label = "Load OriGinalEntity##" + _parentLabel;
    if (ImGui::Button(label.c_str())) {
        std::string directory, filename;
        if (MyFileSystem::SelectFileDialog(kApplicationResourceDirectory, directory, filename, {"ent"}, true)) {
            auto command = std::make_unique<SetterCommand<std::pair<std::string, std::string>>>(&originalEntityFilePath_, std::make_pair(kApplicationResourceDirectory + "/" + directory, filename));
            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
    }

    DragGuiCommand<uint32_t>("Seed##" + _parentLabel, paramData_.openData_.seed, 1, {}, {}, "%d");
    ImGui::Spacing();

    DragGuiCommand<float>("Collision Radius##" + _parentLabel, paramData_.openData_.collisionRadius, 0.01f, 0.f);
    ImGui::Spacing();

    ImGui::Text("Point Density Texture");
    ImGui::SameLine();
    if (AskLoadTextureButton(pointDensityTextureIndex_, "Point Density Texture" + _parentLabel)) {
        std::string directory;
        std::string fileName;
        if (myFs::SelectFileDialog(kApplicationResourceDirectory, directory, fileName, {"png"})) {
            // コマンドを作成
            auto command = std::make_unique<SetterCommand<std::string>>(
                &pointDensityTexturePath_,
                kApplicationResourceDirectory + "/" + directory + "/" + fileName,
                [this](std::string* _newPath) {
                    // テクスチャを読み込んでインデックスを更新する
                    pointDensityTextureIndex_ = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(*_newPath);
                });

            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
    }
    SlideGuiCommand<float>("Density Threshold##" + _parentLabel, paramData_.openData_.densityThreshold, 0.f, 1.f);

    ImGui::Spacing();
    ImGui::Text("Point Volume Params");
    ImGui::SameLine();
    if (AskLoadTextureButton(pointVolumeTextureIndex_, "Point Volume Texture" + _parentLabel)) {
        std::string directory;
        std::string fileName;
        if (myFs::SelectFileDialog(kApplicationResourceDirectory, directory, fileName, {"png"})) {
            // コマンドを作成
            auto command = std::make_unique<SetterCommand<std::string>>(
                &pointVolumeTexturePath_,
                kApplicationResourceDirectory + "/" + directory + "/" + fileName,
                [this](std::string* _newPath) {
                    // テクスチャを読み込んでインデックスを更新する
                    pointVolumeTextureIndex_ = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(*_newPath);
                });

            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
    }

    SlideGuiCommand<float>("Volume Threshold##" + _parentLabel, paramData_.openData_.volumeThreshold, 0.f, 1.f);

    ImGui::Spacing();
    ImGui::Text("Point Mask Params");
    ImGui::SameLine();
    if (AskLoadTextureButton(pointMaskTextureIndex_, "Point Mask Texture" + _parentLabel)) {
        std::string directory;
        std::string fileName;
        if (myFs::SelectFileDialog(kApplicationResourceDirectory, directory, fileName, {"png"})) {
            // コマンドを作成
            auto command = std::make_unique<SetterCommand<std::string>>(
                &pointMaskTexturePath_,
                kApplicationResourceDirectory + "/" + directory + "/" + fileName,
                [this](std::string* _newPath) {
                    // テクスチャを読み込んでインデックスを更新する
                    pointMaskTextureIndex_ = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(*_newPath);
                });

            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
    }

    SlideGuiCommand<float>("Mask Threshold##" + _parentLabel, paramData_.openData_.maskThreshold, 0.f, 1.f);

    ImGui::Spacing();

    DragGuiCommand<uint32_t>("Placement Resolution##" + _parentLabel, paramData_.openData_.placementResolution, 1, {}, {}, "%d");
    DragGuiCommand<uint32_t>("Sample Count##" + _parentLabel, paramData_.openData_.sampleCount, 1, {}, {}, "%d");

    ImGui::Spacing();

    DragGuiVectorCommand<3, float>("Default Scale##" + _parentLabel, defaultScale_, 0.01f);

    DragGuiVectorCommand("World Min XZ##" + _parentLabel, paramData_.openData_.worldMinXZ, 0.01f);
    DragGuiVectorCommand("World Max XZ##" + _parentLabel, paramData_.openData_.worldMaxXZ, 0.01f);

#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const PointPlacementParams& _p) {
    _j = {
        {"seed", _p.paramData_.openData_.seed},
        {"collisionRadius", _p.paramData_.openData_.collisionRadius},
        {"densityThreshold", _p.paramData_.openData_.densityThreshold},
        {"volumeThreshold", _p.paramData_.openData_.volumeThreshold},
        {"maskThreshold", _p.paramData_.openData_.maskThreshold},
        {"placementResolution", _p.paramData_.openData_.placementResolution},
        {"sampleCount", _p.paramData_.openData_.sampleCount},
        {"worldMinXZ", _p.paramData_.openData_.worldMinXZ},
        {"worldMaxXZ", _p.paramData_.openData_.worldMaxXZ},
        {"pointDensityTexturePath", _p.pointDensityTexturePath_},
        {"pointVolumeTexturePath", _p.pointVolumeTexturePath_},
        {"pointMaskTexturePath", _p.pointMaskTexturePath_},
        {"defaultScale", _p.defaultScale_},
        {"originalEntityDirectory", _p.originalEntityFilePath_.first},
        {"originalEntityFileName", _p.originalEntityFilePath_.second}};
}

void from_json(const nlohmann::json& _j, PointPlacementParams& _p) {
    _j.at("seed").get_to(_p.paramData_.openData_.seed);
    _j.at("collisionRadius").get_to(_p.paramData_.openData_.collisionRadius);
    _j.at("densityThreshold").get_to(_p.paramData_.openData_.densityThreshold);
    _j.at("volumeThreshold").get_to(_p.paramData_.openData_.volumeThreshold);
    _j.at("maskThreshold").get_to(_p.paramData_.openData_.maskThreshold);
    _j.at("placementResolution").get_to(_p.paramData_.openData_.placementResolution);
    _j.at("sampleCount").get_to(_p.paramData_.openData_.sampleCount);
    _j.at("worldMinXZ").get_to(_p.paramData_.openData_.worldMinXZ);
    _j.at("worldMaxXZ").get_to(_p.paramData_.openData_.worldMaxXZ);

    _j.at("pointDensityTexturePath").get_to(_p.pointDensityTexturePath_);
    _j.at("pointVolumeTexturePath").get_to(_p.pointVolumeTexturePath_);
    _j.at("pointMaskTexturePath").get_to(_p.pointMaskTexturePath_);

    _j.at("defaultScale").get_to(_p.defaultScale_);

    _j.at("originalEntityDirectory").get_to(_p.originalEntityFilePath_.first);
    _j.at("originalEntityFileName").get_to(_p.originalEntityFilePath_.second);
}
