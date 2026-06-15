#include "BillboardComponent.h"

#ifdef _DEBUG

/// engine
#include "scene/Scene.h"

/// debug
#include "myGui/MyGui.h"

#endif // _DEBUG

using namespace OriGine;

BillboardComponent::BillboardComponent() {}
BillboardComponent::~BillboardComponent() {}

void BillboardComponent::Initialize(Scene* /*_scene*/, const EntityHandle& /*_owner*/) {}
void BillboardComponent::Finalize() {}

void BillboardComponent::Edit([[maybe_unused]] Scene* _scene, [[maybe_unused]] const EntityHandle& _owner, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    ImGui::Text("Billboard Axis :");

    EnumBitmask<BillboardComponent::BillboardAxis> newAxis = axis_;

    const std::pair<BillboardAxis, const char*> flags[] = {
        {BillboardAxis::X, "X"},
        {BillboardAxis::Y, "Y"},
        {BillboardAxis::Z, "Z"},
    };

    for (auto& [flag, name] : flags) {
        std::string label = std::string(name) + "##BillboardComponent" + _parentLabel;
        bool checked      = axis_.HasFlag(flag);
        ImGui::Checkbox(label.c_str(), &checked);
        if (checked) {
            newAxis.SetFlag(flag);
        } else {
            newAxis.ClearFlag(flag);
        }
    }

    if (axis_ != newAxis) {
        auto command = std::make_unique<SetterCommand<EnumBitmask<BillboardComponent::BillboardAxis>>>(
            &axis_,
            newAxis);
        EditorController::GetInstance()->PushCommand(std::move(command));
    }

#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const BillboardComponent& _c) {
    _j = nlohmann::json{
        {"axis", _c.axis_.ToUnderlying()},
    };
}

void from_json(const nlohmann::json& _j, BillboardComponent& _c) {
    _c.axis_ = EnumBitmask<BillboardComponent::BillboardAxis>(_j.at("axis").get<int32_t>());
}
