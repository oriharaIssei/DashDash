#include "TimerForSpriteComponent.h"

/// engine
#define RESOURCE_DIRECTORY
#include "EngineInclude.h"

#include "asset/AssetSystem.h"
#include "scene/Scene.h"
#include "winApp/WinApp.h"

// asset
#include "asset/TextureAsset.h"

/// ECS
// component
#include "component/renderer/Sprite.h"

#ifdef DEBUG
#include "myFileSystem/MyFileSystem.h"
#include "myGui/MyGui.h"
#endif // DEBUG

using namespace OriGine;

void to_json(nlohmann::json& j, const TimerForSpriteComponent& c) {
    j = nlohmann::json{
        {"numbersTexturePath", c.numbersTexturePath},
        {"offset", c.offset},
        {"spriteSizeInteger", c.spriteSizeInteger},
        {"spriteSizeDecimal", c.spriteSizeDecimal},
        {"anchorOnWindow", c.anchorOnWindow},
        {"spriteMarginInteger", c.spriteMarginInteger},
        {"spriteMarginDecimal", c.spriteMarginDecimal},
        {"marginBetweenIntegerAndDecimal", c.marginBetweenIntegerAndDecimal},
        {"digitInteger", c.digitInteger},
        {"digitDecimal", c.digitDecimal}};
}

void from_json(const nlohmann::json& j, TimerForSpriteComponent& c) {
    j.at("numbersTexturePath").get_to(c.numbersTexturePath);
    j.at("offset").get_to(c.offset);
    j.at("spriteSizeInteger").get_to(c.spriteSizeInteger);
    j.at("spriteSizeDecimal").get_to(c.spriteSizeDecimal);
    j.at("spriteMarginInteger").get_to(c.spriteMarginInteger);
    j.at("spriteMarginDecimal").get_to(c.spriteMarginDecimal);
    j.at("marginBetweenIntegerAndDecimal").get_to(c.marginBetweenIntegerAndDecimal);

    j.at("anchorOnWindow").get_to(c.anchorOnWindow);
    j.at("digitInteger").get_to(c.digitInteger);
    j.at("digitDecimal").get_to(c.digitDecimal);
}

void TimerForSpriteComponent::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    if (!this->numbersTexturePath.empty()) {
        textureIndex = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(this->numbersTexturePath);
        this->SettingOnLoadTexture(textureIndex);
    }
}

void TimerForSpriteComponent::Edit([[maybe_unused]] Scene* _scene, EntityHandle /*_owner*/, [[maybe_unused]] const std::string& parentLabel) {

#ifdef DEBUG
    ImGui::Text("Sprite");

    DragGuiVectorCommand("Offset##" + parentLabel, offset, 0.1f);
    DragGuiVectorCommand("SpriteSizeInteger##" + parentLabel, spriteSizeInteger, 0.1f);
    DragGuiVectorCommand("SpriteSizeDecimal##" + parentLabel, spriteSizeDecimal, 0.1f);

    ImGui::Spacing();

    DragGuiVectorCommand("AnchorOnWindow##" + parentLabel, anchorOnWindow, 0.1f);

    ImGui::Spacing();

    DragGuiVectorCommand("SpriteMarginInteger##" + parentLabel, spriteMarginInteger, 0.1f);
    DragGuiVectorCommand("SpriteMarginDecimal##" + parentLabel, spriteMarginDecimal, 0.1f);
    DragGuiVectorCommand("MarginBetweenIntegerAndDecimal##" + parentLabel, marginBetweenIntegerAndDecimal, 0.1f);

    ImGui::Spacing();

    auto askLoadTexture = [&](const std::string& parentLabel) {
        bool askLoad = false;

        std::string label = "Load Texture##" + parentLabel;
        askLoad           = ImGui::Button(label.c_str());
        askLoad |= ImGui::ImageButton(
            reinterpret_cast<ImTextureID>(AssetSystem::GetInstance()->GetManager<TextureAsset>()->GetAsset(textureIndex).srv.GetGpuHandle().ptr),
            ImVec2(18, 18), ImVec2(0, 0), ImVec2(1, 1), 6);
        return askLoad;
    };
    if (askLoadTexture(parentLabel)) {
        std::string directory, fileName;
        std::string textureDefaultDirectory = std::string(kApplicationResourceDirectory) + "/Texture";
        if (myfs::SelectFileDialog(textureDefaultDirectory, directory, fileName, {"png"})) {
            auto command = std::make_unique<SetterCommand<std::string>>(
                &this->numbersTexturePath,
                textureDefaultDirectory + "/" + directory + "/" + fileName,
                [this](std::string*) {
                    textureIndex = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(this->numbersTexturePath);
                    this->SettingOnLoadTexture(textureIndex);
                },
                true);

            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
    }

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Digit");

    InputGuiCommand("Digit Integer" + parentLabel, digitInteger, "%d");
    InputGuiCommand("Digit Decimal" + parentLabel, digitDecimal, "%d");

    RelocationSprites(_scene);

#endif
}

void TimerForSpriteComponent::SettingOnLoadTexture(size_t texIdx) {
    const auto& metaData = AssetSystem::GetInstance()->GetManager<TextureAsset>()->GetAsset(texIdx).metaData;

    if (metaData.width == 0 || metaData.height == 0) {
        LOG_ERROR("Texture metadata is invalid. Width or height is zero.");
        return;
    }

    numbersTextureSize = {static_cast<float>(metaData.width), static_cast<float>(metaData.height)};
    // *0.1 は 10桁であることを想定して
    numberTileSize = {numbersTextureSize[X] * 0.1f, numbersTextureSize[Y]};
}

void TimerForSpriteComponent::RelocationSprites(OriGine::Scene* _scene) {
    auto& sprites = _scene->GetComponents<SpriteRenderer>(spriteEntityHandle_);

    int32_t digitAll   = digitInteger + digitDecimal;
    int32_t spriteSize = static_cast<int32_t>(sprites.size());

    // spriteの数が想定されている数でなかったら、直す
    if (digitAll != spriteSize) {
        if (digitAll < spriteSize) {
            for (int32_t i = digitAll; i < spriteSize; ++i) {
                sprites.pop_back();
            }
        } else if (digitAll > spriteSize) {
            for (int32_t i = spriteSize; i < digitAll; ++i) {
                _scene->AddComponent<SpriteRenderer>(spriteEntityHandle_);
            }
            sprites = _scene->GetComponents<SpriteRenderer>(spriteEntityHandle_);
        }
    }

    // スプライトの生成位置を計算
    OriGine::Vec2f origin = OriGine::Engine::GetInstance()->GetWinApp()->GetWindowSize() * anchorOnWindow;
    origin += offset;

    OriGine::Vec2f uvScale = numberTileSize / numbersTextureSize;

    OriGine::Vec2f pos = origin;

    // 整数部の合計幅
    float widthInteger =
        (digitInteger - 1) * spriteSizeInteger[X] + (digitInteger > 0 ? (digitInteger - 1) * spriteMarginInteger[X] : 0);

    // origin は区切り中央
    // 整数部右端 = origin - widthBetween * 0.5f
    float integerRight = origin[X] - marginBetweenIntegerAndDecimal[X] * 0.5f;

    // 整数部左端
    float integerLeft = integerRight - widthInteger;

    // 最初のスプライト中心
    pos[X] = integerLeft;

    // 整数部
    for (int i = 0; i < digitInteger; ++i) {
        auto* sprite = _scene->GetComponent<SpriteRenderer>(spriteEntityHandle_, i);
        if (sprite) {
            sprite->SetIsRender(true);

            sprite->SetTexture(numbersTexturePath, false);
            sprite->SetAnchorPoint({0.5f, 0.5f});
            sprite->SetSize(spriteSizeInteger);
            sprite->SetTextureSize(numbersTextureSize);
            sprite->SetUVScale(uvScale);
            sprite->SetTranslate(pos);

            pos[X] += spriteSizeInteger[X];
            pos += spriteMarginInteger;
        }
    }

    // 整数部と小数部の間のスペース
    pos += marginBetweenIntegerAndDecimal;

    // 小数部
    for (int i = digitInteger; i < digitAll; ++i) {
        auto* sprite = _scene->GetComponent<SpriteRenderer>(spriteEntityHandle_, i);
        if (sprite) {
            sprite->SetIsRender(true);

            sprite->SetTexture(numbersTexturePath, false);
            sprite->SetAnchorPoint({0.5f, 0.5f});
            sprite->SetSize(spriteSizeDecimal);
            sprite->SetTextureSize(numbersTextureSize);
            sprite->SetUVScale(uvScale);
            sprite->SetTranslate(pos);

            pos[X] += spriteSizeDecimal[X];
            pos += spriteMarginDecimal;
        }
    }
}
