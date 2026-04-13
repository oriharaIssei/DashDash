#include "ButtonGroup.h"

/// engine
#include "scene/Scene.h"

/// ecs
#include "Button.h"

/// util
#include "editor/EditorController.h"
#include "myGui/MyGui.h"

using namespace OriGine;

void ButtonGroup::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}

void ButtonGroup::Edit([[maybe_unused]] OriGine::Scene* _scene, [[maybe_unused]] EntityHandle _owner, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    auto componentArray = _scene->GetComponentRepositoryRef()->GetComponentArray<Button>();
    if (!componentArray) {
        ImGui::Text(" No Button Data.");
        return;
    }

    auto& allButtons = componentArray->GetSlots();

    // 開始番号の設定（必要であれば残す）
    InputGuiCommand("StartButtonIndex##" + _parentLabel, startButtonNumber_, "%2d");
    if (buttonEntityHandles_.empty()) {
        startButtonNumber_ = 0;
    } else {
        startButtonNumber_   = std::clamp(startButtonNumber_, 0, static_cast<int32_t>(buttonEntityHandles_.size()) - 1);
        currentButtonNumber_ = startButtonNumber_;
    }

    std::string label = "Button List##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {

        // =================================================================
        // 1. 現在のリスト表示・並び替え・削除
        // =================================================================
        ImGui::TextDisabled("--- Registered Buttons ---");

        // 削除操作でイテレータが無効になるのを防ぐため、インデックスで回す
        for (size_t i = 0; i < buttonEntityHandles_.size(); ++i) {
            ImGui::PushID(static_cast<int>(i)); // ID被り防止

            // --- 並び替えボタン (Up / Down) ---
            // 上へ移動
            if (i > 0) {
                if (ImGui::ArrowButton("##Up", ImGuiDir_Up)) {
                    std::swap(buttonEntityHandles_[i], buttonEntityHandles_[i - 1]);
                }
                ImGui::SameLine();
            } else {
                // レイアウト調整用のダミー
                ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));
                ImGui::SameLine();
            }

            // 下へ移動
            if (i < buttonEntityHandles_.size() - 1) {
                if (ImGui::ArrowButton("##Down", ImGuiDir_Down)) {
                    std::swap(buttonEntityHandles_[i], buttonEntityHandles_[i + 1]);
                }
                ImGui::SameLine();
            } else {
                ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));
                ImGui::SameLine();
            }

            // --- 情報表示 ---
            OriGine::EntityHandle handle = buttonEntityHandles_[i];
            OriGine::Entity* entity      = _scene->GetEntityRepositoryRef()->GetEntity(handle);
            std::string entityName       = entity ? entity->GetUniqueID() : "INVALID";

            ImGui::Text("[%d] %s", static_cast<int>(i), entityName.c_str());
            ImGui::SameLine();

            // --- 削除ボタン ---
            if (ImGui::Button("Remove")) {
                buttonEntityHandles_.erase(buttonEntityHandles_.begin() + i);
                ImGui::PopID();
                break; // 削除したらループを抜ける
            }

            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::Separator();

        // =================================================================
        // 2. 新規追加 (重複しないものだけリストアップ)
        // =================================================================
        ImGui::TextDisabled("--- Add New Button ---");

        // 追加候補のリスト作成
        std::vector<OriGine::EntityHandle> candidates;
        std::vector<std::string> candidateNames;

        for (const auto& slot : allButtons) {
            // 既に登録済みかチェック (std::vectorなので find が使える)
            auto it = std::find(buttonEntityHandles_.begin(), buttonEntityHandles_.end(), slot.owner);
            if (it == buttonEntityHandles_.end()) {
                // 未登録なら候補に追加
                OriGine::Entity* ent = _scene->GetEntityRepositoryRef()->GetEntity(slot.owner);
                if (ent) {
                    candidates.push_back(slot.owner);
                    candidateNames.push_back(ent->GetUniqueID());
                }
            }
        }

        // コンボボックス表示
        static int selectedIdx = 0;
        if (selectedIdx >= candidates.size())
            selectedIdx = 0;

        if (candidates.empty()) {
            ImGui::Text("No selectable buttons remaining.");
        } else {
            if (ImGui::BeginCombo("Select##AddBtn", candidateNames[selectedIdx].c_str())) {
                for (int i = 0; i < candidates.size(); i++) {
                    const bool isSelected = (selectedIdx == i);
                    if (ImGui::Selectable(candidateNames[i].c_str(), isSelected)) {
                        selectedIdx = i;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();

            // 追加実行
            if (ImGui::Button("Add")) {
                buttonEntityHandles_.push_back(candidates[selectedIdx]);
                selectedIdx = 0; // 追加したらインデックスをリセット
            }
        }

        ImGui::TreePop();
    }
    ImGui::Separator();

    auto editKeyBind = [this](const std::string& _editParentLabel, std::vector<OriGine::Key>& _keyBinds) {
        std::string editorLabel = "";

        for (int i = 0; i < _keyBinds.size(); ++i) {
            ImGui::PushID(i);
            ImGui::Text("Key%d :: ", i);
            ImGui::SameLine();
            editorLabel = "##Key" + std::to_string(i) + _editParentLabel;
            if (ImGui::BeginCombo(editorLabel.c_str(), keyNameMap.find(_keyBinds[i])->second.c_str())) {
                for (auto& key : keyNameMap) {
                    bool isSelected = (_keyBinds[i] == key.first);
                    if (ImGui::Selectable(key.second.c_str(), isSelected)) {
                        auto command = std::make_unique<SetterCommand<OriGine::Key>>(&_keyBinds[i], key.first);
                        OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
        // add
        editorLabel = "Add Key##" + _editParentLabel;
        if (ImGui::Button(editorLabel.c_str())) {
            auto command = std::make_unique<AddElementCommand<std::vector<OriGine::Key>>>(&_keyBinds, OriGine::Key::ESCAPE);
            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
        ImGui::SameLine();
        // remove
        editorLabel = "Remove Key##" + _editParentLabel;
        if (ImGui::Button(editorLabel.c_str())) {
            if (_keyBinds.size() > 0) {
                auto command = std::make_unique<EraseElementCommand<std::vector<OriGine::Key>>>(&_keyBinds, _keyBinds.end() - 1);
                OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
            }
        }
    };
    auto editGamepadButtonBind = [this](const std::string& _editParentLabel, std::vector<OriGine::GamepadButton>& _padButtonBinds) {
        std::string editorLabel = "";
        for (int i = 0; i < _padButtonBinds.size(); ++i) {
            ImGui::PushID(i);
            ImGui::Text("GamepadButton%d :: ", i);
            ImGui::SameLine();
            editorLabel = "##GamepadButton" + std::to_string(i) + _editParentLabel;
            if (ImGui::BeginCombo(editorLabel.c_str(), padButtonNameMap.find(_padButtonBinds[i])->second.c_str())) {
                for (auto& padButton : padButtonNameMap) {
                    bool isSelected = (_padButtonBinds[i] == padButton.first);
                    if (ImGui::Selectable(padButton.second.c_str(), isSelected)) {
                        auto command = std::make_unique<SetterCommand<OriGine::GamepadButton>>(&_padButtonBinds[i], padButton.first);
                        OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
        // add
        editorLabel = "Add GamepadButton##" + _editParentLabel;
        if (ImGui::Button(editorLabel.c_str())) {
            auto command = std::make_unique<AddElementCommand<std::vector<OriGine::GamepadButton>>>(&_padButtonBinds, OriGine::GamepadButton::A);
            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
        ImGui::SameLine();
        // remove
        editorLabel = "Remove GamepadButton##" + _editParentLabel;
        if (ImGui::Button(editorLabel.c_str())) {
            if (_padButtonBinds.size() > 0) {
                auto command = std::make_unique<EraseElementCommand<std::vector<OriGine::GamepadButton>>>(&_padButtonBinds, _padButtonBinds.end() - 1);
                OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
            }
        }
    };

    label = "SelectAddKeyBind##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        editKeyBind("SelectAddKeyBind##" + _parentLabel, selectAddKeys_);
        ImGui::Separator();
        editGamepadButtonBind("SelectAddGamepadButtonBind##" + _parentLabel, selectAddGamepadButtons_);
        ImGui::TreePop();
    }
    label = "SelectSubKeyBind##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        editKeyBind("SelectSubKeyBind##" + _parentLabel, selectSubKeys_);
        ImGui::Separator();
        editGamepadButtonBind("SelectSubGamepadButtonBind##" + _parentLabel, selectSubGamepadButtons_);
        ImGui::TreePop();
    }
    label = "DecideKeyBind##" + _parentLabel;
    if (ImGui::TreeNode(label.c_str())) {
        editKeyBind("DecideKeyBind##" + _parentLabel, decideKeys_);
        ImGui::Separator();
        editGamepadButtonBind("DecideGamepadButtonBind##" + _parentLabel, decideGamepadButtons_);
        ImGui::TreePop();
    }

#endif // _DEBUG
}

void ButtonGroup::Finalize() {}

void to_json(nlohmann::json& j, const ButtonGroup& r) {
    j["startButtonNumber"] = r.startButtonNumber_;

    nlohmann::json buttonNumbersJson = nlohmann::json::array();
    for (auto& buttonNumber : r.buttonEntityHandles_) {
        nlohmann::json buttonNumberJson;
        buttonNumberJson["entityHandle"] = buttonNumber;
        buttonNumbersJson.push_back(buttonNumberJson);
    }
    j["buttonNumbers"] = buttonNumbersJson;

    j["selectAddKeys"]       = r.selectAddKeys_;
    j["selectAddGamepadButtons"] = r.selectAddGamepadButtons_;
    j["selectSubKeys"]       = r.selectSubKeys_;
    j["selectSubGamepadButtons"] = r.selectSubGamepadButtons_;
    j["decideKeys"]          = r.decideKeys_;
    j["decideGamepadButtons"]    = r.decideGamepadButtons_;
}

void from_json(const nlohmann::json& j, ButtonGroup& r) {
    if (j.contains("startButtonNumber")) {
        r.startButtonNumber_ = j.at("startButtonNumber").get<int32_t>();
    }
    if (j.contains("buttonNumbers") && j.at("buttonNumbers").is_array()) {
        r.buttonEntityHandles_.clear();
        for (const auto& buttonNumberJson : j.at("buttonNumbers")) {
            if (buttonNumberJson.contains("entityHandle")) {
                EntityHandle entityID = buttonNumberJson.at("entityHandle").get<EntityHandle>();
                r.buttonEntityHandles_.push_back(entityID);
            }
        }
    }
    if (j.contains("selectAddKeys") && j.at("selectAddKeys").is_array()) {
        r.selectAddKeys_.clear();
        for (const auto& key : j.at("selectAddKeys")) {
            r.selectAddKeys_.push_back(key.get<OriGine::Key>());
        }
    }
    if (j.contains("selectAddGamepadButtons") && j.at("selectAddGamepadButtons").is_array()) {
        r.selectAddGamepadButtons_.clear();
        for (const auto& button : j.at("selectAddGamepadButtons")) {
            r.selectAddGamepadButtons_.push_back(button.get<OriGine::GamepadButton>());
        }
    }
    if (j.contains("selectSubKeys") && j.at("selectSubKeys").is_array()) {
        r.selectSubKeys_.clear();
        for (const auto& key : j.at("selectSubKeys")) {
            r.selectSubKeys_.push_back(key.get<OriGine::Key>());
        }
    }
    if (j.contains("selectSubGamepadButtons") && j.at("selectSubGamepadButtons").is_array()) {
        r.selectSubGamepadButtons_.clear();
        for (const auto& button : j.at("selectSubGamepadButtons")) {
            r.selectSubGamepadButtons_.push_back(button.get<OriGine::GamepadButton>());
        }
    }
    if (j.contains("decideKeys") && j.at("decideKeys").is_array()) {
        r.decideKeys_.clear();
        for (const auto& key : j.at("decideKeys")) {
            r.decideKeys_.push_back(key.get<OriGine::Key>());
        }
    }
    if (j.contains("decideGamepadButtons") && j.at("decideGamepadButtons").is_array()) {
        r.decideGamepadButtons_.clear();
        for (const auto& button : j.at("decideGamepadButtons")) {
            r.decideGamepadButtons_.push_back(button.get<OriGine::GamepadButton>());
        }
    }
}
