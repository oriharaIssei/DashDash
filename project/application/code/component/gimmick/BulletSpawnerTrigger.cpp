#include "BulletSpawnerTrigger.h"

/// engine
#include "scene/Scene.h"

#ifdef _DEBUG
#include "component/gimmick/BulletSpawner.h"
#include "editor/EditorController.h"
#include "myGui/MyGui.h"
#endif // _DEBUG

void BulletSpawnerTrigger::Initialize(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/) {}

void BulletSpawnerTrigger::Finalize() {}

void BulletSpawnerTrigger::Edit([[maybe_unused]] OriGine::Scene* _scene, OriGine::EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    // ── Mode 選択 ──────────────────────────────────────
    if (ImGui::BeginCombo(("Mode##" + _parentLabel).c_str(), kBulletSpawnerTriggerModeNames[static_cast<int>(mode_)])) {
        for (int i = 0; i < 2; ++i) {
            bool isSelected = (mode_ == static_cast<Mode>(i));
            if (ImGui::Selectable(kBulletSpawnerTriggerModeNames[i], isSelected)) {
                auto command = std::make_unique<SetterCommand<Mode>>(&mode_, static_cast<Mode>(i));
                OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();

    // ── ターゲット Entity リスト ──────────────────────────
    // BulletSpawner を持つ Entity の一覧を収集
    std::vector<OriGine::EntityHandle> bulletSpawnerEntityIds;
    for (auto& indexBind : _scene->GetComponentArray<OriGine::BulletSpawner>()->GetSlots()) {
        bulletSpawnerEntityIds.emplace_back(indexBind.owner);
    }

    for (size_t i = 0; i < targetHandles_.size(); ++i) {
        std::string removeLabel = "X##bst" + std::to_string(i) + _parentLabel;
        if (ImGui::Button(removeLabel.c_str())) {
            auto command = std::make_unique<EraseElementCommand<std::vector<OriGine::EntityHandle>>>(&targetHandles_, targetHandles_.begin() + i);
            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
        ImGui::SameLine();

        std::string comboLabel  = "Entity##bst" + std::to_string(i) + _parentLabel;
        OriGine::Entity* entity = _scene->GetEntity(targetHandles_[i]);
        std::string entityName  = "NULL";
        if (entity) {
            entityName = entity->GetUniqueID();
        }
        if (ImGui::BeginCombo(comboLabel.c_str(), entityName.c_str())) {
            for (auto& handle : bulletSpawnerEntityIds) {
                bool isSelected    = (targetHandles_[i] == handle);
                OriGine::Entity* e = _scene->GetEntity(handle);
                if (!e) {
                    continue;
                }
                if (ImGui::Selectable(e->GetUniqueID().c_str(), isSelected)) {
                    auto command = std::make_unique<SetterCommand<OriGine::EntityHandle>>(&targetHandles_[i], handle);
                    OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::Spacing();

    if (ImGui::Button(("Add##bst" + _parentLabel).c_str())) {
        auto command = std::make_unique<AddElementCommand<std::vector<OriGine::EntityHandle>>>(&targetHandles_, OriGine::EntityHandle{});
        OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
    }
#endif // _DEBUG
}
