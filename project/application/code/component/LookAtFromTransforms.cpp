#include "LookAtFromTransforms.h"

#ifdef _DEBUG

/// engine
#include "scene/Scene.h"

/// ECS
// entity
#include "entity/Entity.h"
// component
#include "component/transform/Transform.h"

/// debug
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

LookAtFromTransforms::LookAtFromTransforms() {}
LookAtFromTransforms::~LookAtFromTransforms() {}

void LookAtFromTransforms::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}
void LookAtFromTransforms::Finalize() {}

void LookAtFromTransforms::Edit([[maybe_unused]] Scene* _scene, [[maybe_unused]] EntityHandle _owner, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    ImGui::Text("Rotate Axis :");

    std::string label                                           = "X##LookAtFromTransforms" + _parentLabel;
    EnumBitmask<LookAtFromTransforms::RotateAxis> newRotateAxis = rotateAxis;
    bool isChecked                                              = rotateAxis.HasFlag(RotateAxis::X);
    ImGui::Checkbox(label.c_str(), &isChecked);
    if (isChecked) {
        newRotateAxis.SetFlag(RotateAxis::X);
    } else {
        newRotateAxis.ClearFlag(RotateAxis::X);
    }
    label     = "Y##LookAtFromTransforms" + _parentLabel;
    isChecked = rotateAxis.HasFlag(RotateAxis::Y);
    ImGui::Checkbox(label.c_str(), &isChecked);
    if (isChecked) {
        newRotateAxis.SetFlag(RotateAxis::Y);
    } else {
        newRotateAxis.ClearFlag(RotateAxis::Y);
    }
    label     = "Z##LookAtFromTransforms" + _parentLabel;
    isChecked = rotateAxis.HasFlag(RotateAxis::Z);
    ImGui::Checkbox(label.c_str(), &isChecked);
    if (isChecked) {
        newRotateAxis.SetFlag(RotateAxis::Z);
    } else {
        newRotateAxis.ClearFlag(RotateAxis::Z);
    }
    if (rotateAxis != newRotateAxis) {
        auto command = std::make_unique<SetterCommand<EnumBitmask<LookAtFromTransforms::RotateAxis>>>(
            &rotateAxis,
            newRotateAxis);
        OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
    }

    // Transform コンポーネントを持つエンティティ一覧を取得
    auto transformCompArray    = _scene->GetComponentArray<OriGine::Transform>();
    auto& componentLocationMap = transformCompArray->GetComponentLocationMap();
    auto& slots                = transformCompArray->GetSlots();
    Entity* fromEntity         = nullptr;
    if (fromTransformComp.IsValid()) {
        const auto& currentLocationItr = componentLocationMap.find(fromTransformComp.uuid);
        if (currentLocationItr != componentLocationMap.end()) {
            const auto& currentLocation = currentLocationItr->second;
            fromEntity                  = _scene->GetEntity(slots[currentLocation.entitySlot].owner);
        }
    }

    std::string entityLabel = fromEntity != nullptr ? fromEntity->GetUniqueID() : "NULL";
    label                   = "FromTransformEntity##" + _parentLabel;
    if (ImGui::BeginCombo(label.c_str(), entityLabel.c_str())) {
        for (auto& [compHandle, locationData] : componentLocationMap) {
            auto& compSlot = slots[locationData.entitySlot];

            Entity* entity = _scene->GetEntity(compSlot.owner);
            if (!entity) {
                continue;
            }

            int32_t compIndex = 0;
            for (auto& comp : compSlot.components) {
                entityLabel     = std::format("{}_[{}]", entity->GetUniqueID(), compIndex++);
                bool isSelected = (fromTransformComp == comp.GetHandle());
                if (ImGui::Selectable(entityLabel.c_str(), isSelected)) {
                    auto command = std::make_unique<SetterCommand<ComponentHandle>>(&fromTransformComp, comp.GetHandle());
                    OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
                }
            }
        }
        ImGui::EndCombo();
    }

    Entity* toEntity = nullptr;
    if (toTransformComp.IsValid()) {
        const auto& currentLocationItr = componentLocationMap.find(toTransformComp.uuid);
        if (currentLocationItr != componentLocationMap.end()) {
            const auto& currentLocation = currentLocationItr->second;
            toEntity                    = _scene->GetEntity(slots[currentLocation.entitySlot].owner);
        }
    }
    entityLabel = toEntity != nullptr ? toEntity->GetUniqueID() : "NULL";
    label       = "ToTransformEntity##" + _parentLabel;
    if (ImGui::BeginCombo(label.c_str(), entityLabel.c_str())) {
        for (auto& [compHandle, locationData] : componentLocationMap) {
            auto& compSlot = slots[locationData.entitySlot];

            Entity* entity = _scene->GetEntity(compSlot.owner);
            if (!entity) {
                continue;
            }

            int32_t compIndex = 0;
            for (auto& comp : compSlot.components) {
                entityLabel     = std::format("{}_[{}]", entity->GetUniqueID(), compIndex++);
                bool isSelected = (toTransformComp == comp.GetHandle());
                if (ImGui::Selectable(entityLabel.c_str(), isSelected)) {
                    auto command = std::make_unique<SetterCommand<ComponentHandle>>(&toTransformComp, comp.GetHandle());
                    OriGine::EditorController::GetInstance()->PushCommand(std::move(command));
                }
            }
        }
        ImGui::EndCombo();
    }

#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const LookAtFromTransforms& _c) {
    _j = nlohmann::json{
        {"rotateAxis", _c.rotateAxis.ToUnderlying()},
        {"fromTransformComp", _c.fromTransformComp},
        {"toTransformComp", _c.toTransformComp},
    };
}

void from_json(const nlohmann::json& _j, LookAtFromTransforms& _c) {
    _c.rotateAxis        = EnumBitmask<LookAtFromTransforms::RotateAxis>(_j.at("rotateAxis").get<int32_t>());
    _c.fromTransformComp = _j.at("fromTransformComp").get<ComponentHandle>();
    _c.toTransformComp   = _j.at("toTransformComp").get<ComponentHandle>();
}
