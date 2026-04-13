#include "ObstacleSpawnGroupComponent.h"

/// engine
#include "scene/Scene.h"

#ifdef _DEBUG
#include "component/gimmick/Obstacle.h"
#include "editor/EditorController.h"
#include "myGui/MyGui.h"
#endif // _DEBUG

void ObstacleSpawnGroupComponent::Initialize(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/) {}

void ObstacleSpawnGroupComponent::Finalize() {}

void ObstacleSpawnGroupComponent::Edit([[maybe_unused]] OriGine::Scene* _scene, OriGine::EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    // Obstacleコンポーネントを持つEntity一覧を収集
    std::vector<OriGine::EntityHandle> obstacleEntityIds;
    for (auto& indexBind : _scene->GetComponentArray<Obstacle>()->GetSlots()) {
        obstacleEntityIds.emplace_back(indexBind.owner);
    }

    for (size_t i = 0; i < obstacleHandles_.size(); ++i) {
        std::string removeLabel = "X##" + std::to_string(i) + _parentLabel;
        if (ImGui::Button(removeLabel.c_str())) {
            auto command = std::make_unique<EraseElementCommand<std::vector<OriGine::EntityHandle>>>(&obstacleHandles_, obstacleHandles_.begin() + i);
            OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
        }
        ImGui::SameLine();

        std::string comboLabel  = "Entity##" + std::to_string(i) + _parentLabel;
        OriGine::Entity* entity = _scene->GetEntity(obstacleHandles_[i]);
        std::string entityName  = "NULL";
        if (entity) {
            entityName = entity->GetUniqueID();
        }
        if (ImGui::BeginCombo(comboLabel.c_str(), entityName.c_str())) {
            for (auto& handle : obstacleEntityIds) {
                bool isSelected    = (obstacleHandles_[i] == handle);
                OriGine::Entity* e = _scene->GetEntity(handle);
                if (!e) {
                    continue;
                }
                if (ImGui::Selectable(e->GetUniqueID().c_str(), isSelected)) {
                    auto command = std::make_unique<SetterCommand<OriGine::EntityHandle>>(&obstacleHandles_[i], handle);
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

    // 追加
    if (ImGui::Button(("Add##" + _parentLabel).c_str())) {
        auto command = std::make_unique<AddElementCommand<std::vector<OriGine::EntityHandle>>>(&obstacleHandles_, OriGine::EntityHandle{});
        OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
    }
#endif // _DEBUG
}
